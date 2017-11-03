#include <cblas.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

// Using BLAS

double *create_empty_matrix(int m, int n){
	double *result = malloc(sizeof(double) * m * n);
	return result;
}

double *create_matrix_with_random_values(int m, int n){
	double *mat = create_empty_matrix(m, n);
	int i, j;
	for(i = 0; i < m * n; i++){
		mat[i] = rand() % 5; // numbers in range 0-4
		mat[i] -= 2; // adjust range to -2-+2
	}
	return mat;
}


long long time_matmul(int size){
	double *a = create_matrix_with_random_values(size, size);
	double *b = create_matrix_with_random_values(size, size);
	double *c = create_empty_matrix(size, size);
	struct timeval start_time;
	struct timeval end_time;
	long long time_taken;
	gettimeofday(&start_time, NULL);
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size, size, size, 1.0, a, size, b, size, 1.0, c, size);
	gettimeofday(&end_time, NULL);
	time_taken = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
	free(a);
	free(b);
	free(c);
	return time_taken;
}

#define NUM_ITERATIONS 20
const int size = 2000;

int main(int argc, char *argv[]){
	int i;
	long long time_taken = 0;
	for(i = 0; i < NUM_ITERATIONS; i++){
		time_taken += time_matmul(size);
		
	}
	time_taken = time_taken / NUM_ITERATIONS;
	printf("For size=%d the average time taken across %d runs was: %lld microseconds\n", size, NUM_ITERATIONS, time_taken);
	return 0;
}
