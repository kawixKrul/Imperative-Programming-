#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SIZE 40

#define TEST 0    // 1 - dla testow,  0 - dla oceny automatycznej

void read_vec(double x[], size_t n) {
  for(size_t i=0; i<n; ++i)  scanf("%lf",x++);
}

void print_vec(const double x[], size_t n) {
  for(size_t i=0; i<n; ++i) printf("%.4f ",x[i]);
  printf("\n");
}

void read_mat(double A[][SIZE], size_t m, size_t n) {
  for(size_t i=0; i<m; ++i) {
    for(size_t j=0; j<n; ++j)  scanf("%lf",&A[i][j]);
  }
}

void print_mat(const double A[][SIZE], size_t m, size_t n) {
  for(size_t i=0; i<m; ++i) {
    for(size_t j=0; j<n; ++j)  printf("%.4f ",A[i][j]);
    printf("\n");
  }
}


double triangulate(double A[][SIZE], size_t n, double eps) {
    double detA = 1.0;

    for (int k = 0; k < n; ++k) {
        int max_row = k;
        double max_value = fabs(A[k][k]);
        for (int i = k + 1; i < n; i++) {
            if (fabs(A[i][k]) > max_value) {
                max_row = i;
                max_value = fabs(A[i][k]);
            }
        }
        if (max_row != k) {
            for (int j = k; j < n; ++j) {
                double temp = A[k][j];
                A[k][j] = A[max_row][j];
                A[max_row][j] = temp;
            }
            detA *= -1.0;
        }
        if (fabs(A[k][k]) < eps) {
            detA = 0.0;
        }
        for (int i = k + 1; i < n; ++i) {
            double factor = A[i][k] / A[k][k];
            for (int j = k + 1; j < n; j++) {
                A[i][j] -= factor * A[k][j];
            }
            A[i][k] = 0.0;
        }
        detA *= A[k][k];
    }
    return detA;
}

// 5.2.1 Triangularyzacja, wyznacznik i rozwiazanie Ax=b dla  macierzy kwadratowej.
// Wersja rozszerzona: Wybor wiersza z maksymalna waroscia |elementu glownego|
// Przy wymianie wierszy nalezy zastosowac wetor permutacji indeksow wierszy.
// Jezeli maksymalna wartosc |elementu glownego| < eps, to wyznacznik = 0.
// Zwraca wyznacznik det. Jezeli =0,  to triangularyzacja moze byc niedokonczona.
// Jezeli wyznacznik != 0, i b,x != NULL,
// to w wektorze x umieszcza rozwiazanie ukladu rownan Ax=b.
double gauss(double A[][SIZE], const double b[], double x[], size_t n, double eps) {
    double copy[n][SIZE];
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                copy[i][j] = A[i][j];
            }
        }
        for (int i = 0; i < n; ++i) {
            copy[i][k] = b[i];
        }
        double detC = triangulate(copy, n, eps);
        x[k] = detC;
    }
    double detA = triangulate(A, n, eps);
    for (int i = 0; i < n; ++i) {
        x[i] = x[i]/detA;
    }
    return detA;
}

// 5.2.2
// Zwraca wyznacznik i w tablicy B macierz odwrotna (jezlei wyznacznik != 0)
// Jezeli maksymalna bezwzgledna wartosc elementu glownego < eps,
// to funkcja zwraca wartosc wyznacznika det = 0.
// Funkcja zmienia wartosci takze w tablicy A.

double matrix_inv(double A[][SIZE], double B[][SIZE], size_t n, double eps) {
    double det = 1.0, max, tmp;
    size_t p;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            B[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }

    // Gaussian elimination with partial pivoting
    for (int k = 0; k < n; ++k) {
        // find pivot row
        max = fabs(A[k][k]);
        p = k;
        for (int i = k + 1; i < n; ++i) {
            if (fabs(A[i][k]) > max) {
                max = fabs(A[i][k]);
                p = i;
            }
        }
        if (max < eps) {
            // matrix is singular
            return 0.0;
        }
        if (p != k) {
            // Swap rows k and p
            for (int j = 0; j < n; ++j) {
                tmp = A[k][j];
                A[k][j] = A[p][j];
                A[p][j] = tmp;
                tmp = B[k][j];
                B[k][j] = B[p][j];
                B[p][j] = tmp;
            }
            det = -det;
        }
        //reduce row k
        tmp = A[k][k];
        det *= tmp;
        for (int j = 0; j < n; ++j) {
            A[k][j] /= tmp;
            B[k][j] /= tmp;
        }
        //eliminate all other entries in column k
        for (int i = 0; i < n; ++i) {
            if (i != k) {
                tmp = A[i][k];
                for (int j = 0; j < n; ++j) {
                    A[i][j] -= tmp * A[k][j];
                    B[i][j] -= tmp * B[k][j];
                }
            }
        }
    }

    return det;
}

int main(void) {

    double A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
    double b[SIZE], x[SIZE], det, eps = 1.e-13;
	int to_do;
	size_t m,n,p,q;
	if(TEST) printf("Wpisz nr zadania ");
	scanf ("%d", &to_do);
	switch (to_do) {
    case 1:
      if(TEST) printf("Wpisz liczbe wierszy i kolumn mac. kwadratowej: ");
      scanf("%d", &n);
      if(TEST) printf("Wpisz macierz A (wierszami): ");
      read_mat(A,n,n);
      if(TEST) printf("Wpisz wektor b: ");
      read_vec(b,n);
      det = gauss(A, b, x, n, eps);
      printf("%.4f\n",det);
      if(det) print_vec(x,n);
      break;
    case 2:
      if(TEST) printf("Wpisz rozmiar macierzy n = ");
      scanf("%d",&n);
      if(TEST) printf("Wpisz elementy macierzy (wierszami): ");
      read_mat(A,n,n);
      det = matrix_inv(A,B,n,eps);
      printf("%.4f\n",det);
      if(det) print_mat(B,n,n);
      break;
    default:
      printf("NOTHING TO DO FOR %d\n", to_do);
	}
	return 0;
}
