#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <numeric>
#include <ctime>

#ifdef _WIN32
  #include <windows.h>
  #include <psapi.h>
#else
  #include <sys/resource.h>
#endif

namespace fs = std::filesystem;

// Matrix class with contiguous storage
typedef double Scalar;
class Matrix {
public:
    explicit Matrix(size_t n) : n_(n), data_(n * n) {}

    Scalar& operator()(size_t i, size_t j) {
        return data_[i * n_ + j];
    }
    const Scalar& operator()(size_t i, size_t j) const {
        return data_[i * n_ + j];
    }

    size_t size() const { return n_; }

private:
    size_t n_;
    std::vector<Scalar> data_;
};

// Generate an n×n matrix of uniform [0,1) doubles
Matrix generate_matrix(size_t n) {
    Matrix M(n);
    std::mt19937_64 rng(std::random_device{}());
    std::uniform_real_distribution<Scalar> dist(0.0, 1.0);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            M(i, j) = dist(rng);
        }
    }
    return M;
}

// Naïve O(n^3) multiply
Matrix multiply(const Matrix& A, const Matrix& B) {
    size_t n = A.size();
    Matrix C(n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            Scalar sum = 0.0;
            for (size_t k = 0; k < n; ++k) {
                sum += A(i, k) * B(k, j);
            }
            C(i, j) = sum;
        }
    }
    return C;
}

// Cross-platform peak memory usage (MB)
double get_peak_memory_mb() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<double>(pmc.PeakWorkingSetSize) / (1024.0 * 1024.0);
    }
    return 0.0;
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
  #ifdef __APPLE__
    return static_cast<double>(usage.ru_maxrss) / (1024.0 * 1024.0);
  #else
    return static_cast<double>(usage.ru_maxrss) / 1024.0;
  #endif
#endif
}

void benchmark_size(size_t n, int runs, std::ofstream& out) {
    std::cout << "Benchmarking size " << n << "x" << n << "...\n";

    // Pre-generate matrices
    Matrix A = generate_matrix(n);
    Matrix B = generate_matrix(n);

    // Warm-up (not timed)
    multiply(A, B);

    std::vector<double> wall_times;
    std::vector<double> cpu_times;
    std::vector<double> mem_usages;

    for (int r = 0; r < runs; ++r) {
        auto cpu0 = std::clock();
        auto t0 = std::chrono::high_resolution_clock::now();

        Matrix C = multiply(A, B);

        auto t1 = std::chrono::high_resolution_clock::now();
        auto cpu1 = std::clock();

        double elapsed = std::chrono::duration<double>(t1 - t0).count();
        double cpu_sec = double(cpu1 - cpu0) / CLOCKS_PER_SEC;
        double mem_mb = get_peak_memory_mb();

        wall_times.push_back(elapsed);
        cpu_times.push_back(cpu_sec);
        mem_usages.push_back(mem_mb);
    }

    // Discard first measurement
    if (!wall_times.empty()) {
        wall_times.erase(wall_times.begin());
        cpu_times.erase(cpu_times.begin());
        mem_usages.erase(mem_usages.begin());
    }

    // Compute averages
    double avg_wall = std::accumulate(wall_times.begin(), wall_times.end(), 0.0) / wall_times.size();
    double avg_cpu  = std::accumulate(cpu_times.begin(),  cpu_times.end(),  0.0) / cpu_times.size();
    double avg_mem  = std::accumulate(mem_usages.begin(), mem_usages.end(), 0.0) / mem_usages.size();

    // Write CSV line and flush
    out << n << "," << avg_wall << "," << avg_cpu << "," << avg_mem << "\n";
    out.flush();

    std::cout << "  -> avg wall: " << avg_wall << "s, cpu: " << avg_cpu << "s, mem: " << avg_mem << "MB\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Starting C++ benchmark with Matrix class...\n";

    // Determine project root from executable path
    fs::path exePath = fs::absolute(argv[0]);
    // exePath: .../cpp/build/benchmark.exe
    // go up three levels: build -> cpp -> project root
    fs::path projectRoot = exePath.parent_path().parent_path().parent_path();
    std::cout << "Executable: " << exePath << "\n";
    std::cout << "Project root: " << projectRoot << "\n";

    // Prepare root-level results directory
    fs::path resultsDir = projectRoot / "results";
    std::cout << "Creating results directory: " << resultsDir << "\n";
    fs::create_directories(resultsDir);

    // CSV output path in root/results
    fs::path csvFile = resultsDir / "benchmark_results_cpp.csv";
    std::cout << "CSV file: " << csvFile << "\n";
    std::ofstream out(csvFile);
    if (!out) {
        std::cerr << "Error: cannot write to " << csvFile << "\n";
        return 1;
    }

    // CSV header
    out << "size,avg_wall_s,avg_cpu_s,avg_mem_mb\n";
    out.flush();

    std::vector<size_t> sizes = {128, 256, 512, 1024, 2048};
    const int runs = 4;  // discard first measurement

    for (auto n : sizes) {
        benchmark_size(n, runs, out);
    }

    std::cout << "C++ benchmark complete. CSV saved to: " << csvFile << "\n";
    return 0;
}
