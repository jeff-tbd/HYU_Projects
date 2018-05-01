//2013012148_LeeJaeil_508
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int n, m;
	int cnt = 0;
	int* A;
	int* B;
	scanf("%d %d", &n, &m);
	A = (int*)malloc(n*sizeof(int));
	B = (int*)malloc(m*sizeof(int));
	for (int i = 0; i < n; i++)
	{
		scanf("%d", &A[i]);
	}
	for (int i = 0; i < m; i++)
	{
		scanf("%d", &B[i]);
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (A[i] == B[j])
				cnt++;
		}
	}
	printf("%d", cnt);
	//
	free(A);
	free(B);
	return 0;
}