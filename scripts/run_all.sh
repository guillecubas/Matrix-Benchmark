#!/usr/bin/env bash
set -euo pipefail

# ------------------------------------------------------------------
# Script: run_all.sh
# Purpose: Build and run C++, Java, and Python matrix benchmarks,
#          then generate comparison plots—all without virtual environments.
# Usage:
#   chmod +x scripts/run_all.sh
#   ./scripts/run_all.sh
# ------------------------------------------------------------------

# 1. Determine project root
dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(dirname "$dir")"
echo "Project root: $project_root"


mkdir -p "$project_root/results"
echo
#!/bin/bash
set -e  # Salir si hay error

# ===== C++ Benchmark =====
echo "[C++] Iniciando proceso..."

# Verificar project_root
if [ -z "$project_root" ]; then
    project_root="$(pwd)"
    echo "[C++] project_root definido como: $project_root"
fi

echo "[C++] Cambiando a directorio C++..."
cd "$project_root/cpp" || {
    echo "[ERROR] No se puede acceder a $project_root/cpp"
    exit 1
}

echo "[C++] Directorio actual: $(pwd)"
echo "[C++] Contenido del directorio:"
ls -la

echo "[C++] Creando directorio build..."
mkdir -p build

echo "[C++] Verificando archivos fuente..."
if [ ! -f "src/benchmark.cpp" ]; then
    echo "[ERROR] No se encuentra src/benchmark.cpp"
    exit 1
fi

if [ ! -f "src/matrix.cpp" ]; then
    echo "[ERROR] No se encuentra src/matrix.cpp"
    exit 1
fi

echo "[C++] Compilando..."
g++ -std=c++17 -O3 -march=native src/benchmark.cpp src/matrix.cpp -o build/benchmark.exe -lpsapi 2>&1
if [ $? -ne 0 ]; then
    echo "[C++] Compilation failed!"
    exit 1
fi

echo "[C++] Compilación exitosa. Ejecutando benchmark..."
./build/benchmark.exe 2>&1
exit_code=$?

echo "[C++] Benchmark terminado con código: $exit_code"
if [ $exit_code -ne 0 ]; then
    echo "[C++] Execution failed!"
    exit 1
fi

echo "[C++] Done. Results -> $project_root/results/benchmark_results_cpp.csv"
# ===== Java Benchmark =====
echo "[Java] Compiling and running..."
cd "$project_root/java/src"
javac MatrixBenchmark.java
cd "$project_root"
java -cp java/src MatrixBenchmark

echo "[Java] Done. Results -> $project_root/results/benchmark_results_java.csv"
echo

# ===== Python Benchmark =====
echo "[Python] Running benchmark..."
cd "$project_root/python"

# Install dependencies (global install)
echo "[Python] Installing dependencies..."
pip install --upgrade pip
if [[ -f requirements.txt ]]; then
  pip install -r requirements.txt
fi

# Run Python benchmark
python src/bench.py

echo "[Python] Done. Results -> $project_root/results/benchmark_results_python.csv"
echo

# ===== Generate Plots =====
echo "[Plots] Generating comparison plots..."
python "$project_root/plots/generate_plots.py"
echo "[Plots] Done. Charts saved in $project_root/plots"
echo

echo "All benchmarks and plots complete. Results directory: $project_root/results"
