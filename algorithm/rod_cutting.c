//2013012148_LeeJaeil_508
#include <stdio.h>
#include <stdlib.h>

void extended_bottom_up_cut_rod(int* p, int* r, int* s, int N)
{
	int q;
	r[0] = 0;
	for (int j = 1; j <= N; j++)
	{
		q = -2147483647;
		for (int i = 1; i <= j; i++)
		{
			if (q < p[i] + r[j - i])
			{
				q = p[i] + r[j - i];
				s[j] = i;
			}
		}
		r[j] = q;
	}
}

void print_cut_rod_solution(int *p, int *s, int n)
{
	while (n > 0)
	{
		printf("%d ", s[n]);
		n = n - s[n];
	}
}

int main()
{
	int N;
	int* p;
	int* s;
	int* r;
	scanf("%d", &N);
	p = (int*)malloc(sizeof(int)*(N+1));
	r = (int*)malloc(sizeof(int)*(N+1));
	s = (int*)malloc(sizeof(int)*(N+1));
	p[0] = 0;
	for (int i = 1; i <= N; i++)
	{
		scanf("%d", &p[i]);
	}
	extended_bottom_up_cut_rod(p, r, s, N);
	printf("%d\n", r[N]);
	print_cut_rod_solution(p, s, N);
	return 0;
}