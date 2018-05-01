//2013012148_LeeJaeil_508
#include <stdio.h>
#include <stdlib.h>

void Merge(int *A, int p, int q, int r)
{
	int n1 = q - p + 1;
	int n2 = r - q;
	int i = 1, j = 1;
	int* L = (int*)malloc(sizeof(int)*(n1 + 2));
	int* R = (int*)malloc(sizeof(int)*(n2 + 2));

	for (i = 1; i <= n1; i++)
	{
		L[i] = A[p + i - 1];
	}

	for (j = 1; j <= n2; j++)
	{
		R[j] = A[q + j];
	}
	L[n1 + 1] = 100000000000;
	R[n2 + 1] = 100000000000;
	i = 1; j = 1;

	for (int k = p; k <= r; k++)
	{
		if (L[i] <= R[j])
		{
			A[k] = L[i++];
		}
		else
		{
			A[k] = R[j++];
		}
	}
}

void MergeSort(int* A, int p, int r)
{
	if (p < r)
	{
		int q = (p + r) / 2;
		MergeSort(A, p, q);
		MergeSort(A, q + 1, r);
		Merge(A, p, q, r);
	}
}

int main()
{
	int n;
	int* arr;
	scanf("%d", &n);
	arr = (int*)malloc(sizeof(int)*n);
	for (int i = 0; i < n; i++)
	{
		scanf("%d", &arr[i]);
		//
	}
	MergeSort(arr, 0, n - 1);
	for (int i = n - 1; i >= 0; i--)
	{
		printf("%d\n", arr[i]);
	}
	return 0;
}