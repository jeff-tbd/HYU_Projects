//2013012148_LeeJaeil_508
#include <stdio.h>
#include <stdlib.h>

void Counting(int* input, int* C, int m, int n)
{
	for (int i = 0; i < m + 1; i++)
	{
		C[i] = 0;
	}
	for (int i = 0; i < n; i++)
	{
		C[input[i]]++;
	}
	for (int i = 1; i < m + 1; i++)
	{
		C[i] = C[i] + C[i - 1];
	}
}

int main()
{
	int n = 0, m = 0, k = 0;
	int* A;
	int* B;
	int* C;
	int* input;
	scanf("%d %d %d", &n, &m, &k);
	A = (int*)malloc(k*sizeof(int));
	B = (int*)malloc(k*sizeof(int));
	C = (int*)malloc((m + 1)*sizeof(int));
	input = (int*)malloc(n*sizeof(int));
	for (int i = 0; i < k; i++)
	{
		scanf("%d %d", &A[i], &B[i]);
	}
	for (int i = 0; i < n; i++)
	{
		scanf("%d", &input[i]);
	}

	Counting(input, C, m, n);
	for (int i = 0; i < k; i++)
	{
		if (A[i] != 0)
			printf("%d\n", C[B[i]] - C[A[i] - 1]);
		else
			printf("%d\n", C[B[i]]);
	}
	return 0;
}