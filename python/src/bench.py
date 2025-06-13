# File: python/src/bench.py

import random
import time
import tracemalloc
import psutil
from pathlib import Path

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

def run_size_benchmark(size: int, iterations: int) -> dict[str, float]:
    """
    Benchmark multiply_matrices for a given matrix size.
    Returns average wall-time, CPU-time, and memory usage (MB).
    """
    process = psutil.Process()

    # Pre-generate matrices
    matrix_A = create_random_matrix(size)
    matrix_B = create_random_matrix(size)

    # Warm-up run (not timed)
    multiply_matrices(matrix_A, matrix_B)

    elapsed_times: list[float] = []
    cpu_times: list[float] = []
    memory_usages: list[float] = []

    for _ in range(iterations):
        tracemalloc.start()
        cpu_start = process.cpu_times()
        time_start = time.perf_counter()

        multiply_matrices(matrix_A, matrix_B)

        time_end = time.perf_counter()
        cpu_end = process.cpu_times()
        _, peak_memory = tracemalloc.get_traced_memory()
        tracemalloc.stop()

        elapsed_times.append(time_end - time_start)
        cpu_duration = (cpu_end.user + cpu_end.system) - (cpu_start.user + cpu_start.system)
        cpu_times.append(cpu_duration)
        memory_usages.append(peak_memory / (1024 * 1024))  # bytes to MB

    # Discard first iteration
    elapsed_times.pop(0)
    cpu_times.pop(0)
    memory_usages.pop(0)

    return {
        'avg_time_s': sum(elapsed_times) / len(elapsed_times),
        'avg_cpu_s': sum(cpu_times) / len(cpu_times),
        'avg_mem_mb': sum(memory_usages) / len(memory_usages)
    }

def main() -> None:
    print("Starting Python benchmark...")
    matrix_sizes = [128, 256, 512, 1024, 2048]
    num_runs = 4  # first run is discarded

    # Locate project root by finding 'results' directory
    current_dir = Path(__file__).resolve().parent
    project_root = current_dir
    while not (project_root / 'results').exists():
        project_root = project_root.parent

    results_dir = project_root / 'results'
    results_dir.mkdir(exist_ok=True)

    output_file = results_dir / 'benchmark_results_python.csv'
    with open(output_file, 'w', encoding='utf-8') as file:
        file.write("size,avg_time_s,avg_cpu_s,avg_mem_mb\n")
        for size in matrix_sizes:
            print(f"Benchmarking {size}x{size}...")
            stats = run_size_benchmark(size, num_runs)

            line = (
                f"{size},"
                f"{stats['avg_time_s']:.6f},"
                f"{stats['avg_cpu_s']:.6f},"
                f"{stats['avg_mem_mb']:.2f}\n"
            )
            file.write(line)
            file.flush()

            # Use plain ASCII for the arrow
            print(
                f"  -> avg time: {stats['avg_time_s']:.3f}s, "
                f"cpu: {stats['avg_cpu_s']:.3f}s, "
                f"mem: {stats['avg_mem_mb']:.1f}MB"
            )

    print("Python benchmark complete. CSV at", output_file)

if __name__ == '__main__':
    main()
