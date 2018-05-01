//2013012148_LeeJaeil_508
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void findPath(int** C, char* small, char* path, int m, int n, int i)
{
	if (m == 0 || n == 0)
	{
		path[i] = '\0';
		return;
	}
	if (C[m][n] == 3)
	{
		path[i] = small[m-1];
		findPath(C, small, path, m - 1, n - 1, i + 1);
		//
	}
	else if (C[m][n] == 2)
	{
		findPath(C, small, path, m - 1, n, i);
		//
	}
	else if (C[m][n] == 1)
	{
		findPath(C, small, path, m, n - 1, i);
		//
	}
}

int main()
{
	int m, n, tmp;
	char A[501];
	char B[501];
	char *big, *small;
	int **M, **C;
	char* path;
	scanf("%s", A);
	scanf("%s", B);
	m = strlen(B);
	n = strlen(A);
	if (m > n)
	{
		small = A;
		big = B;
		tmp = m;
		m = n;
		n = tmp;
	}
	else
	{
		small = B;
		big = A;
	}

	M = (int**)malloc(sizeof(int*)*2);
	for (int i = 0; i <2; i++)
	{
		M[i] = (int*)malloc(sizeof(int)*(n+1));
	}

	C = (int**)malloc(sizeof(int*)*(m + 1));
	for (int i = 0; i <= m; i++)
	{
		C[i] = (int*)malloc(sizeof(int)*(n + 1));
	}
	
	path = (char*)malloc(sizeof(char)*(m + 1));
	///////////////////////
	for (int j = 0; j <= n; j++)
	{
		M[0][j] = 0;
	}
	for (int i = 1; i <= m; i++)
	{
		M[1][0] = 0;
		for (int j = 1; j <= n; j++)
		{
			if (small[i-1] == big[j-1])
			{
				M[1][j] = M[0][j - 1] + 1;
				C[i][j] = 3;
			}
			else if (M[1][j - 1] <= M[0][j])
			{
				M[1][j] = M[0][j];
				C[i][j] = 2;
			}
			else if (M[1][j - 1] > M[0][j])
			{
				M[1][j] = M[1][j - 1];
				C[i][j] = 1;
			}
			//end j
		} 
		for (int k = 1; k <= n; k++)
			M[0][k] = M[1][k];
		//end i
	}
	

	findPath(C, small, path, m, n, 0);
	for (int i = strlen(path) - 1; i >= 0; i--)
	{
		printf("%c", path[i]);
	}
	
	return 0;
}