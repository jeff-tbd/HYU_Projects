//2013012148_LeeJaeil_508
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int N, e1, e2, x1, x2;
	int end;
	int *a1, *a2, *t1, *t2;
	int **L;
	int *path;
	int s[2][2];
	scanf("%d", &N);
	scanf("%d %d", &e1, &e2);
	scanf("%d %d", &x1, &x2);
	path = (int*)malloc(sizeof(int)*(N));
	L = (int**)malloc(sizeof(int*)*2);
	L[0] = (int*)malloc(sizeof(int)*(N - 1));
	L[1] = (int*)malloc(sizeof(int)*(N - 1));
	a1 = (int*)malloc(sizeof(int)*(N));
	a2 = (int*)malloc(sizeof(int)*(N));
	t1 = (int*)malloc(sizeof(int)*(N-1));
	t2 = (int*)malloc(sizeof(int)*(N-1));
	for (int i = 0; i < N; i++)
	{
		scanf("%d", &a1[i]);
	}
	for (int i = 0; i < N; i++)
	{
		scanf("%d", &a2[i]);
	}
	for (int i = 0; i < N-1; i++)
	{
		scanf("%d", &t1[i]);
	}
	for (int i = 0; i < N-1; i++)
	{
		scanf("%d", &t2[i]);
	}
	//make s and L array
	for (int i = 0; i < N; i++)
	{
		if (i == 0)
		{
			s[0][0] = e1 + a1[0];
			s[1][0] = e2 + a2[0];
		} else {
			int tmp1 = s[0][0]+a1[i];
			int tmp2 = s[1][0]+t2[i-1] + a1[i];
			int tmp3 = s[1][0] + a2[i];
			int tmp4 = s[0][0] + a2[i] + t1[i - 1];
			if (tmp1 <= tmp2)
			{
				s[0][0] = tmp1;
				L[0][i - 1] = 0;
			}else{
				s[0][0] = tmp2;
				L[0][i - 1] = 1;
			}
			///////
			if (tmp3 <= tmp4)
			{
				s[1][0] = tmp3;
				L[1][i - 1] = 1;
			}
			else{
				s[1][0] = tmp4;
				L[1][i - 1] = 0;
			}
		}
	}
	if (s[0][0] + x1 < s[1][0] + x2)
	{
		printf("%d\n", s[0][0] + x1);
		end = 0;
	}
	else
	{
		printf("%d\n", s[1][0] + x2);
		end = 1;
	}
	path[N - 1] = end;
	for (int i = N-2; i >= 0; i--)
	{
		end = L[end][i];
		path[i] = end;
	}
	for (int i = 0; i < N; i++)
	{
		printf("%d %d\n", path[i]+1, i + 1);
	}


	return 0;
}