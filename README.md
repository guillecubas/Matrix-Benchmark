# Matrix Benchmark

Compare dense matrix multiplication performance across **C++**, **Java**, and **Python**.

## 📁 Project Layout

```
matrix-benchmark/
├── cpp/          C++ source, build, results
├── java/         Java source, results
├── python/       Python scripts, results
├── results/      All benchmark CSVs
├── scripts/      Orchestration (run_all.sh, run_*.sh)
├── plots/        Generated charts
└── .gitignore    Files/folders to ignore
```

## ⚡ Quick Start

Ensure you have a C++ compiler, JDK, and Python installed.

1. **Full pipeline**

   ```bash
   chmod +x scripts/run_all.sh
   ./scripts/run_all.sh
   ```

2. **C++ only**

   ````bash
   cd cpp
   # Compile with optimizations
   g++ -std=c++17 -O3 -march=native src/benchmark.cpp -o build/benchmark.exe -lpsapi
   # Run
   mkdir -p ../results
   ./build/benchmark.exe
   ```bash
   cd cpp && mkdir -p build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ../src && make
   ./benchmark
   ````

3. **Java only**

   ```bash
   cd java/src
   javac MatrixBenchmark.java
   cd ../../
   java -cp java/src MatrixBenchmark
   ```

4. **Python only**

   ```bash
   pip install -r requirements.txt
   python python/src/bench.py
   ```

## 📊 Results & Plots

* **CSV outputs** in `results/`
* **Charts** (PNG) in `plots/` via `generate_plots.py`

---

*MIT License*
