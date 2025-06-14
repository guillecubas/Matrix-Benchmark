

import java.io.BufferedWriter;
import java.io.IOException;
import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;
import java.nio.file.*;
import java.util.Locale;
import java.util.concurrent.ThreadLocalRandom;

public class MatrixBenchmark {
    public static void main(String[] args) {
        System.out.println("Starting Java Matrix Multiplication Benchmark...");

        Path resultsDir = Paths.get("..", "results").toAbsolutePath().normalize();
        Path outputFile = resultsDir.resolve("benchmark_results_java.csv");


        try {
            Files.createDirectories(resultsDir);
        } catch (IOException e) {
            System.err.println("Failed to create results directory: " + e.getMessage());
            return;
        }

        int[] sizes = {128, 256, 512, 1024, 2048};
        int runs = 3;

        Locale.setDefault(Locale.US);

        try (BufferedWriter writer = Files.newBufferedWriter(outputFile)) {
            writer.write("size,avg_time_s,avg_cpu_s,avg_mem_mb");
            writer.newLine();

            for (int n : sizes) {
                double totalTime = 0.0, totalCpu = 0.0, totalMem = 0.0;

                for (int r = 0; r < runs; r++) {
                    double[][] A = generateMatrix(n);
                    double[][] B = generateMatrix(n);

                    System.gc();
                    Thread.sleep(50);

                    ThreadMXBean bean = ManagementFactory.getThreadMXBean();
                    boolean cpuSupported = bean.isCurrentThreadCpuTimeSupported();
                    long cpuStart = cpuSupported ? bean.getCurrentThreadCpuTime() : 0L;
                    long timeStart = System.nanoTime();

                    double[][] C = MatrixUtils.multiply(A, B);

                    long timeEnd = System.nanoTime();
                    long cpuEnd = cpuSupported ? bean.getCurrentThreadCpuTime() : 0L;

                    double elapsed = (timeEnd - timeStart) / 1e9;
                    double cpuTime = cpuSupported ? (cpuEnd - cpuStart) / 1e9 : 0.0;
                    long usedMem = (Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory()) / (1024 * 1024);

                    totalTime += elapsed;
                    totalCpu += cpuTime;
                    totalMem += usedMem;
                }

                String line = String.format(Locale.US, "%d,%.6f,%.6f,%.2f",
                        n, totalTime / runs, totalCpu / runs, totalMem / runs);
                writer.write(line);
                writer.newLine();
            }

            System.out.println("Benchmark complete: " + outputFile);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static double[][] generateMatrix(int n) {
        double[][] matrix = new double[n][n];
        ThreadLocalRandom rand = ThreadLocalRandom.current();
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                matrix[i][j] = rand.nextDouble();
        return matrix;
    }
}
