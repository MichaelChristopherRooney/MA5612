#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// matrix a is l x m
// matrix b is m x n
// matrix c is l x n
void mat_mul(double **a, double **b, double **c, int l, int m, int n){
	int i, j, k;
	for(i = 0; i < l; i++){
		for(j = 0; j < n; j++){
			double sum = 0.0;
			for(k = 0; k < m; k++){
				sum += a[i][k] * b[k][j];
			}
			c[i][j] = sum;
		}
	}
}

double **create_empty_matrix(int m, int n){
	double **result = malloc(sizeof(double) * m);
	double *mat = malloc(sizeof(double) * m * n);
	int i;
	for(i = 0; i < m; i++){
		result[i] = &(mat[i * n]);
	}
	return result;
}

double **create_matrix_with_random_values(int m, int n){
	double **mat = create_empty_matrix(m, n);
	int i, j;
	for(i = 0; i < m; i++){
		for(j = 0; j < n; j++){
			mat[i][j] = rand() % 5; // numbers in range 0-4
			mat[i][j] -= 2; // adjust range to -2-+2
		}
	}
	return mat;
}

void print_matrix(double **mat, int m, int n){
	int i, j;
	for(i = 0; i < m; i++){
		for(j = 0; j < n; j++){
			printf("%f\t ", mat[i][j]);
		}
		printf("\n");
	}
}

int parse_args(char *argv[], int *l, int *m, int *n){
	*l = atoi(argv[1]);
	*m = atoi(argv[2]);
	*n = atoi(argv[3]);
	if(*l == 0 || *m == 0 || *n == 0){
		return 1;
	}
	return 0;
}

int main(int argc, char *argv[]){
	if(argc != 4){
		printf("Expected 3 ints passed as arguments!\n");
		return 1;
	}
	int l, m, n;
	int result = parse_args(argv, &l, &m, &n);
	if(result == 1){
		printf("Error parsing integer arguments.\n");
		return 1;
	}
	double **a = create_matrix_with_random_values(l, m);
	double **b = create_matrix_with_random_values(m, n);
	double **c = create_empty_matrix(l, n);
	mat_mul(a, b, c, l, m, n);
	printf("a\n");
	print_matrix(a, l, m);
	printf("b\n");
	print_matrix(b, m, n);
	printf("c\n");
	print_matrix(c, l, n);
	return 0;
}
