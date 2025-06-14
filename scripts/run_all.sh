#!/usr/bin/env bash
set -euo pipefail

# ------------------------------------------------------------------
# Script: run_all.sh
# Purpose: Build and run C++, Java, and Python matrix benchmarks,
#          then generate comparison plots—all without virtual environments.
# ------------------------------------------------------------------

# Determinar raíz del proyecto
dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(dirname "$dir")"
echo "[INFO] Project root: $project_root"

# Directorios
cpp_dir="$project_root/cpp"
java_src_dir="$project_root/java/src"
python_dir="$project_root/python"
results_dir="$project_root/results"
plots_dir="$project_root/plots"

mkdir -p "$results_dir"
echo

### ===== C++ Benchmark =====
echo "[C++] Iniciando proceso..."

if [ ! -d "$cpp_dir" ]; then
    echo "[ERROR] No se encontró el directorio: $cpp_dir"
    exit 1
fi

pushd "$cpp_dir" > /dev/null

mkdir -p build

if [[ ! -f src/benchmark.cpp ]]; then
    echo "[ERROR] Falta src/benchmark.cpp"
    exit 1
fi
if [[ ! -f src/matrix.cpp ]]; then
    echo "[ERROR] Falta src/matrix.cpp"
    exit 1
fi

echo "[C++] Compilando..."
g++ -std=c++17 -O3 -march=native src/benchmark.cpp src/matrix.cpp -o build/benchmark.exe -lpsapi
echo "[C++] Ejecutando benchmark..."
./build/benchmark.exe
echo "[C++] Hecho. Resultados -> $results_dir/benchmark_results_cpp.csv"
popd > /dev/null
echo

# ===== Java Benchmark =====
echo "[Java] Compilando y ejecutando..."

if [ ! -d "$java_src_dir" ]; then
    echo "[ERROR] No se encontró el directorio: $java_src_dir"
    exit 1
fi

pushd "$java_src_dir" > /dev/null

if [[ ! -f MatrixBenchmark.java ]]; then
    echo "[ERROR] Falta MatrixBenchmark.java"
    exit 1
fi

javac MatrixBenchmark.java
echo "[Java] Ejecutando..."
java MatrixBenchmark
echo "[Java] Hecho. Resultados -> $results_dir/benchmark_results_java.csv"
popd > /dev/null
echo

# ===== Python Benchmark =====
echo "[Python] Ejecutando benchmark..."

if [ ! -d "$python_dir" ]; then
    echo "[ERROR] No se encontró el directorio: $python_dir"
    exit 1
fi

pushd "$python_dir" > /dev/null


if [[ ! -f src/bench.py ]]; then
    echo "[ERROR] Falta src/bench.py"
    exit 1
fi

python src/bench.py
echo "[Python] Hecho. Resultados -> $results_dir/benchmark_results_python.csv"
popd > /dev/null
echo

# ===== Plots =====
echo "[Plots] Generando gráficos de comparación..."

if [[ ! -f "$plots_dir/generate_plots.py" ]]; then
    echo "[ERROR] No se encuentra generate_plots.py en $plots_dir"
    exit 1
fi

python "$plots_dir/generate_plots.py"
echo "[Plots] Hecho. Gráficos guardados en $plots_dir"
echo

echo "✅ Todos los benchmarks y gráficos completados. Resultados en: $results_dir"
