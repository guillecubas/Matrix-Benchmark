

public class MatrixUtils {

    // Multiply two n x n matrices A and B
    public static double[][] multiply(double[][] A, double[][] B) {
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
