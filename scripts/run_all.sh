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

# 2. Ensure top-level results directory exists
mkdir -p "$project_root/results"
echo

# ===== C++ Benchmark =====
echo "[C++] Building and running..."
cd "$project_root/cpp"
mkdir -p build
g++ -std=c++17 -O3 -march=native src/benchmark.cpp -o build/benchmark.exe -lpsapi
RUN_CMD="build/benchmark.exe"

# Run C++ benchmark
$RUN_CMD

echo "[C++] Done. Results -> $project_root/results/benchmark_results_cpp.csv"
echo

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
