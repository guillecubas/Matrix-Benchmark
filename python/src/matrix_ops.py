import random

def create_random_matrix(size: int) -> list[list[float]]:
    """
    Generate a size×size matrix of random floats in [0,1).
    """
    random.seed(42)
    return [[random.random() for _ in range(size)] for _ in range(size)]

def multiply_matrices(A: list[list[float]], B: list[list[float]]) -> list[list[float]]:
    """
    Multiply two square matrices A and B.
    """
    n = len(A)
    result = [[0.0] * n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            total = 0.0
            for k in range(n):
                total += A[i][k] * B[k][j]
            result[i][j] = total
    return result
