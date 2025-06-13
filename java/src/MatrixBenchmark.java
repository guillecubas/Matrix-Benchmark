import java.io.BufferedWriter;
import java.io.IOException;
import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Locale;
import java.util.concurrent.ThreadLocalRandom;

public class MatrixBenchmark {
    public static void main(String[] args) {
        System.out.println("Starting Java Matrix Multiplication Benchmark...");

        // 1. Determine project root as the current working directory
        Path projectRoot = Paths.get(System.getProperty("user.dir"))
                                .toAbsolutePath().normalize();
        System.out.println("Project root: " + projectRoot);

        // 2. Prepare top-level results directory
        Path resultsDir = projectRoot.resolve("results");
        System.out.println("Creating results directory: " + resultsDir);
        try {
            Files.createDirectories(resultsDir);
        } catch (IOException e) {
            System.err.println("Failed to create results directory: " + e.getMessage());
            return;
        }

        // 3. Define output CSV path in root/results
        Path outputFile = resultsDir.resolve("benchmark_results_java.csv");
        System.out.println("Output file: " + outputFile);

        // 4. Run benchmarks and write CSV
        int[] sizes = {128, 256, 512, 1024, 2048};
        int runs = 3;

        // Ensure all String.format uses '.' as decimal separator
        Locale.setDefault(Locale.US);

        try (BufferedWriter writer = Files.newBufferedWriter(outputFile)) {
            // CSV header
            writer.write("size,avg_time_s,avg_cpu_s,avg_mem_mb");
            writer.newLine();
            writer.flush();

            for (int n : sizes) {
                System.out.println("Benchmarking matrix size " + n + "x" + n + "...");

                double totalTime = 0.0;
                double totalCpu  = 0.0;
                double totalMem  = 0.0;

                for (int r = 1; r <= runs; r++) {
                    System.out.println("  Run " + r + "/" + runs);

                    // Generate random matrices
                    double[][] A = generateMatrix(n);
                    double[][] B = generateMatrix(n);

                    // Warm-up GC to reduce noise
                    System.gc();
                    Thread.sleep(50);

                    ThreadMXBean bean = ManagementFactory.getThreadMXBean();
                    boolean cpuSupported = bean.isCurrentThreadCpuTimeSupported();
                    long cpuStart = cpuSupported ? bean.getCurrentThreadCpuTime() : 0L;
                    long timeStart = System.nanoTime();

                    // Matrix multiplication
                    double[][] C = multiply(A, B);

                    long timeEnd = System.nanoTime();
                    long cpuEnd  = cpuSupported ? bean.getCurrentThreadCpuTime() : 0L;

                    double elapsed = (timeEnd - timeStart) / 1e9;
                    double cpuSec  = cpuSupported ? (cpuEnd - cpuStart) / 1e9 : 0.0;
                    long   usedMem = (Runtime.getRuntime().totalMemory()
                                     - Runtime.getRuntime().freeMemory())
                                     / (1024L * 1024L);

                    totalTime += elapsed;
                    totalCpu  += cpuSec;
                    totalMem  += usedMem;

                    System.out.printf("    Time: %.3fs, CPU: %.3fs, Mem: %dMB%n",
                                      elapsed, cpuSec, usedMem);
                }

                // Compute averages
                double avgTime = totalTime / runs;
                double avgCpu  = totalCpu  / runs;
                double avgMem  = totalMem  / runs;

                // Write to CSV and flush
                String line = String.format(
                    Locale.US,
                    "%d,%.6f,%.6f,%.2f",
                    n, avgTime, avgCpu, avgMem
                );
                writer.write(line);
                writer.newLine();
                writer.flush();

                System.out.printf("  Average - Time: %.3fs, Memory: %.2fMB%n",
                                  avgTime, avgMem);
            }

            System.out.println("Benchmark complete! Results saved to: " + outputFile);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // Generate an n x n matrix of random doubles in [0,1)
    private static double[][] generateMatrix(int n) {
        double[][] M = new double[n][n];
        ThreadLocalRandom rnd = ThreadLocalRandom.current();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                M[i][j] = rnd.nextDouble();
            }
        }
        return M;
    }

    // Multiply two n x n matrices A and B
    private static double[][] multiply(double[][] A, double[][] B) {
        int n = A.length;
        double[][] C = new double[n][n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                double sum = 0.0;
                for (int k = 0; k < n; k++) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
        return C;
    }
}
