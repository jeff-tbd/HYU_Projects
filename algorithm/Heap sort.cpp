//2013012148_LeeJaeil_508
#include <stdio.h>
#include <stdlib.h>

int n;

void MaxHeapify(int* A, int i)
{
	int largest, temp;
	int L = i * 2;
	int R = i * 2 + 1;
	if (L <= n && A[L] > A[i])
	{
		largest = L;
	}
	else
	{
		largest = i;
	}
	if (R <= n && A[R] > A[largest])
	{
		largest = R;
	}
	if (largest != i)
	{
		temp = A[i];
		A[i] = A[largest];
		A[largest] = temp;
		MaxHeapify(A, largest);
	}
	//
}

void BuildMaxHeap(int* A)
{
	for (int i = (n / 2); i >= 1; i--)
	{
		MaxHeapify(A, i);
	}
	//
}

void HeapSort(int* A, int m)
{
	int temp;
	int length = n;
	BuildMaxHeap(A);
	for (int i = length; i > length - m; i--)
	{
		temp = A[1];
		A[1] = A[i];
		A[i] = temp;
		n = n - 1;
		MaxHeapify(A, 1);
	}
}


int main()
{
	int m, length;
	int* arr;
	scanf("%d %d", &n, &m);
	length = n;
	arr = (int*)malloc(sizeof(int)*(n + 1));
	for (int i = 1; i <= length; i++)
	{
		scanf("%d", &arr[i]);
	}
	HeapSort(arr, m);
	for (int i = length; i > length - m; i--)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
	for (int i = 1; i <= length - m; i++)
	{
		printf("%d ", arr[i]);
	}
	return 0;
}