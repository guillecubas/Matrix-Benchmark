// matrix.hpp
#pragma once
#include <vector>
#include <cstddef>

typedef double Scalar;

class Matrix {
public:
    explicit Matrix(size_t n);
    Scalar& operator()(size_t i, size_t j);
    const Scalar& operator()(size_t i, size_t j) const;
    size_t size() const;

private:
    size_t n_;
    std::vector<Scalar> data_;
};

Matrix generate_matrix(size_t n);
Matrix multiply(const Matrix& A, const Matrix& B);
double get_peak_memory_mb();
