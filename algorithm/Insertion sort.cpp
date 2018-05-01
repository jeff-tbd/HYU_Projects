//2013012148 LeeJaeil 508
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
void insertionSort(int arr[], int n)
{
	int i, j, key;
	for (i = 1; i < n; i++)
	{
		key = arr[i];
		j = i - 1;

		while (j >= 0 && arr[j] < key)
		{
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = key;
	}
}

void printArr(int arr[], int n)
{
	for (int i = 0; i < n; i++)
		printf("%d\n", arr[i]);
}

int main()
{
	int* arr;
	int N;
	scanf("%d", &N);
	arr = (int*)malloc(sizeof(int) * N);
	for (int i = 0; i<N; i++)
	{
		scanf("%d", &arr[i]);
	}
	insertionSort(arr, N);
	printArr(arr, N);
	return 0;
}