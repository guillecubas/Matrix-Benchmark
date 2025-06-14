# Matrix Benchmark

Compare dense matrix multiplication performance across **C++**, **Java**, and **Python**.

## 📁 Project Layout

```
matrix-benchmark/
├── cpp/          C++ source, build
├── java/         Java source
├── python/       Python scripts
├── results/      All benchmark CSVs
├── scripts/      Orchestration (run_all.sh)
├── plots/        Generated charts
└── .gitignore    Files/folders to ignore
```

## ⚡ Quick Start


1. **Full pipeline**

   ```bash
   chmod +x scripts/run_all.sh
   ./scripts/run_all.sh
   ```

2. **C++ only**

   ````bash
   cd cpp
   # Compile with optimizations
   g++ -std=c++17 -O3 -march=native src/benchmark.cpp src/matrix.cpp -o build/benchmark.exe -lpsapi 2>&1
   # Run
   mkdir -p ../results
   ./build/benchmark.exe


3. **Java only**

   ```bash
   cd java/src
   javac MatrixBenchmark.java
   cd ../../
   java -cp java/src MatrixBenchmark
   ```

4. **Python only**

   ```bash
   python python/src/bench.py
   ```

## 📊 Results & Plots

* **CSV outputs** in `results/`
* **Charts** (PNG) in `plots/` via `generate_plots.py`

