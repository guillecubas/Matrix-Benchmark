#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>

#ifdef _WIN32
  #include <windows.h>
  #include <psapi.h>
#else
  #include <sys/resource.h>
#endif

namespace fs = std::filesystem;

// === Production code: matrix operations ===
static std::vector<std::vector<double>> generate_matrix(size_t n) {
    std::mt19937_64 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::vector<std::vector<double>> M(n, std::vector<double>(n));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            M[i][j] = dist(rng);
    return M;
}

static std::vector<std::vector<double>> matrix_multiply(
    const std::vector<std::vector<double>>& A,
    const std::vector<std::vector<double>>& B) {
    size_t n = A.size();
    std::vector<std::vector<double>> C(n, std::vector<double>(n, 0.0));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k)
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
    return C;
}

// Cross-platform function to get peak memory usage (MB)
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
    // On Linux, ru_maxrss is in KB, on macOS it's in bytes
    #ifdef __APPLE__
        return static_cast<double>(usage.ru_maxrss) / (1024.0 * 1024.0);
    #else
        return static_cast<double>(usage.ru_maxrss) / 1024.0;
    #endif
#endif
}

// === Benchmarking function ===
void benchmark_size(size_t n, int runs, std::ofstream& out) {
    std::cout << "Benchmarking matrix size " << n << "x" << n << "..." << std::endl;
    
    double total_time = 0.0;
    double total_cpu = 0.0;
    double total_mem = 0.0;

    for (int r = 0; r < runs; ++r) {
        std::cout << "  Run " << (r + 1) << "/" << runs << std::endl;
        
        auto A = generate_matrix(n);
        auto B = generate_matrix(n);

        auto t0 = std::chrono::high_resolution_clock::now();
        std::clock_t c0 = std::clock();

        auto C = matrix_multiply(A, B);

        std::clock_t c1 = std::clock();
        auto t1 = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> dt = t1 - t0;
        double elapsed = dt.count();
        double cpu_sec = double(c1 - c0) / CLOCKS_PER_SEC;
        double peak_mb = get_peak_memory_mb();

        total_time += elapsed;
        total_cpu += cpu_sec;
        total_mem += peak_mb;
        
        std::cout << "    Time: " << elapsed << "s, Memory: " << peak_mb << "MB" << std::endl;
    }

    double avg_time = total_time / runs;
    double avg_cpu = total_cpu / runs;
    double avg_mem = total_mem / runs;

    out << n << ","
        << avg_time << ","
        << avg_cpu << ","
        << avg_mem << "\n";
    
    std::cout << "  Average - Time: " << avg_time << "s, Memory: " << avg_mem << "MB" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "Starting C++ Matrix Multiplication Benchmark..." << std::endl;
    
    // Get current working directory
    fs::path cwd = fs::current_path();
    std::cout << "Current working directory: " << cwd << std::endl;
    
    // Try multiple strategies for results directory
    fs::path results_dir;
    
    // Strategy 1: Try parent directory
    if (cwd.has_parent_path()) {
        results_dir = cwd.parent_path() / "results";
    } else {
        // Strategy 2: Use current directory
        results_dir = cwd / "results";
    }
    
    std::cout << "Creating results directory: " << results_dir << std::endl;
    
    try {
        fs::create_directories(results_dir);
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        // Fallback to current directory
        results_dir = cwd;
        std::cout << "Using current directory instead: " << results_dir << std::endl;
    }

    // Prepare output file
    fs::path output_file = results_dir / "benchmark_results_cpp.csv";
    std::cout << "Output file: " << output_file << std::endl;
    
    std::ofstream out(output_file);
    if (!out) {
        std::cerr << "Failed to open " << output_file << " for writing" << std::endl;
        return 1;
    }

    out << "size,avg_time_s,avg_cpu_s,avg_mem_mb\n";

    std::vector<size_t> sizes = {128, 256, 512, 1024, 2048};
    int runs = 3;
    
    std::cout << "Running benchmark with " << runs << " runs per size" << std::endl;
    
    for (auto n : sizes) {
        benchmark_size(n, runs, out);
        out.flush(); // Ensure data is written immediately
    }

    out.close();
    std::cout << "Benchmark complete! Results saved to: " << output_file << std::endl;
    return 0;
}