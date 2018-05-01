//2013012148_LeeJaeil_508
#include <stdio.h>
#include <stdlib.h>

void printParen(int** S, int i, int j)
{
	int bound = S[i][j];
	if (i + 1 == j)
	{
		printf("%d %d ", i, j);
		return;
	}
	else if (bound == i)
	{
		printf("%d ", i);
		printf("( ");
		printParen(S, bound + 1, j);
		printf(") ");
	}
	else if ((bound + 1) == j)
	{
		printf("( ");
		printParen(S, i, bound);
		printf(") ");
		printf("%d ", j);
	}
	else
	{
		printf("( ");
		printParen(S, i, bound);
		printf(") ");
		printf("( ");
		printParen(S, bound + 1, j);
		printf(") ");
	}
}


int main()
{
	int N;
	int *A, **S, **M;
	int min, tmp, position;

	scanf("%d", &N);
	A = (int*)malloc(sizeof(int)*(N + 1));
	M = (int**)malloc(sizeof(int*)*(N+1));
	S = (int**)malloc(sizeof(int*)*N);
	for (int i = 0; i <= N; i++)
	{
		scanf("%d", &A[i]);
	}
	//////////////////////////////////
	for (int i = 0; i <= N; i++)
	{
		M[i] = (int*)malloc(sizeof(int)*(N+1));
	}
	for (int i = 0; i < N; i++)
	{
		S[i] = (int*)malloc(sizeof(int)*(N+1));
	}
	for (int i = 0; i <= N-1; i++)
	{
		if (i == 0)
		{
			for (int j = 1; j <= N; j++)
			{
				M[j][j] = 0;
			}
		}
		else if (i == 1)
		{
			for (int j = 1; j <=N-i; j++)
			{
				M[j][j + 1] = A[j - 1] * A[j] * A[j + 1];
				S[j][j + 1] = j;
			}
		}
		else
		{
			for (int j = 1; j <= N - i; j++)
			{
				min = 2147483647;
				tmp = -1;
				position = -1;
				for (int k = 0; k < i; k++)
				{
					tmp = M[j][j + k] + M[j + k + 1][j + i] + A[j - 1] * A[j + k] * A[j + i];
					if (min > tmp)
					{
						min = tmp;
						position = j+k;
					}
				}
				M[j][j + i] = min;
				S[j][j + i] = position;
			}
			//end j
		}
		//end i
	}
	
	printf("%d\n", M[1][N]);
	printf("( ");
	printParen(S, 1, N);
	printf(") ");
	return 0;
}