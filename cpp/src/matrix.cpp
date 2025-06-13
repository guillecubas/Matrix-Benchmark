// matrix.cpp
#include "matrix.hpp"
#include <random>

#ifdef _WIN32
  #include <windows.h>
  #include <psapi.h>
#else
  #include <sys/resource.h>
#endif

Matrix::Matrix(size_t n) : n_(n), data_(n * n) {}

Scalar& Matrix::operator()(size_t i, size_t j) {
    return data_[i * n_ + j];
}

const Scalar& Matrix::operator()(size_t i, size_t j) const {
    return data_[i * n_ + j];
}

size_t Matrix::size() const {
    return n_;
}

Matrix generate_matrix(size_t n) {
    Matrix M(n);
    std::mt19937_64 rng(std::random_device{}());
    std::uniform_real_distribution<Scalar> dist(0.0, 1.0);
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            M(i, j) = dist(rng);
    return M;
}

Matrix multiply(const Matrix& A, const Matrix& B) {
    size_t n = A.size();
    Matrix C(n);
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j) {
            Scalar sum = 0.0;
            for (size_t k = 0; k < n; ++k)
                sum += A(i, k) * B(k, j);
            C(i, j) = sum;
        }
    return C;
}

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
