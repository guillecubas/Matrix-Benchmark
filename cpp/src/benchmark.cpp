// benchmark.cpp
#include "matrix.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <numeric>

namespace fs = std::filesystem;

void benchmark_size(size_t n, int runs, std::ofstream& out) {
    std::cout << "Benchmarking size " << n << "x" << n << "...\n";

    Matrix A = generate_matrix(n);
    Matrix B = generate_matrix(n);
    multiply(A, B);  // Warm-up

    std::vector<double> wall_times, cpu_times, mem_usages;

    for (int r = 0; r < runs; ++r) {
        auto cpu0 = std::clock();
        auto t0 = std::chrono::high_resolution_clock::now();
        Matrix C = multiply(A, B);
        auto t1 = std::chrono::high_resolution_clock::now();
        auto cpu1 = std::clock();

        wall_times.push_back(std::chrono::duration<double>(t1 - t0).count());
        cpu_times.push_back(double(cpu1 - cpu0) / CLOCKS_PER_SEC);
        mem_usages.push_back(get_peak_memory_mb());
    }

    wall_times.erase(wall_times.begin());
    cpu_times.erase(cpu_times.begin());
    mem_usages.erase(mem_usages.begin());

    double avg_time = std::accumulate(wall_times.begin(), wall_times.end(), 0.0) / wall_times.size();
    double avg_cpu  = std::accumulate(cpu_times.begin(),  cpu_times.end(),  0.0) / cpu_times.size();
    double avg_mem  = std::accumulate(mem_usages.begin(), mem_usages.end(), 0.0) / mem_usages.size();

    out << n << "," << avg_time << "," << avg_cpu << "," << avg_mem << "\n";
    out.flush();

    std::cout << "  -> avg time: " << avg_time << "s, cpu: " << avg_cpu << "s, mem: " << avg_mem << "MB\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Starting C++ benchmark with Matrix class...\n";

    fs::path exePath = fs::absolute(argv[0]);
    fs::path projectRoot = exePath.parent_path().parent_path().parent_path();
    fs::path resultsDir = projectRoot / "results";
    fs::create_directories(resultsDir);

    fs::path csvFile = resultsDir / "benchmark_results_cpp.csv";
    std::ofstream out(csvFile);
    if (!out) {
        std::cerr << "Error: cannot write to " << csvFile << "\n";
        return 1;
    }

    out << "size,avg_time_s,avg_cpu_s,avg_mem_mb\n";

    std::vector<size_t> sizes = {128, 256, 512, 1024, 2048};
    const int runs = 4;

    for (auto n : sizes)
        benchmark_size(n, runs, out);

    std::cout << "C++ benchmark complete. CSV saved to: " << csvFile << "\n";
    return 0;
}
