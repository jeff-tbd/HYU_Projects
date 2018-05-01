//2013012148_LeeJaeil_508
#include <stdio.h>
#include <stdlib.h>

void Selection(int* A, int n, int m)
{
	int min;
	int temp;
	for (int i = 0; i < m; i++)
	{
		min = i;
		for (int j = i + 1; j < n; j++)
		{
			if (A[j] < A[min])
				min = j;
		}
		temp = A[i];
		A[i] = A[min];
		A[min] = temp;
	}
	//
}

int main()
{
	int n, m;
	int* arr;
	scanf("%d %d", &n, &m);
	arr = (int*)malloc(sizeof(int)*n);
	for (int i = 0; i < n; i++)
	{
		scanf("%d", &arr[i]);
	}
	Selection(arr, n, m);
	for (int i = 0; i < n; i++)
	{
		printf("%d\n", arr[i]);
	}
	return 0;
}