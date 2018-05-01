
/* Driver for routine eigsrt */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define NRANSI
#include "nr.h"
#include "nrutil.h"

#define NP 9

int main(void)
{
	int i,j,nrot;
	static float c[NP][NP];
	float *d,**v,**e;

	srand(time(NULL));

	//input random number by using gaussian distribution
	for (int i = 0; i < NP; i++)
	{
		for (int j = i; j < NP; j++)
		{
			c[i][j] = c[j][i] = gasdev(0, 1);
			//
		}
		
	}

	printf("print matrix\n");
	for (int i = 0; i < NP; i++)
	{
		for (int j = 0; j < NP; j++)
		{
			printf("%f ", c[i][j]);
		}
		printf("\n");
	}

	d=vector(1,NP);
	v=matrix(1,NP,1,NP);
	e=convert_matrix(&c[0][0],1,NP,1,NP);
	printf("****** Finding Eigenvectors ******\n");
	jacobi(e,NP,d,v,&nrot);
	printf("unsorted eigenvectors:\n");
	for (i=1;i<=NP;i++) {
		printf("eigenvalue %3d = %12.6f\n",i,d[i]);
		printf("eigenvector:\n");
		for (j=1;j<=NP;j++) {
			printf("%12.6f",v[j][i]);
			if ((j % 5) == 0) printf("\n");
		}
		printf("\n");
	}
	printf("\n****** Sorting Eigenvectors ******\n\n");
	eigsrt(d,v,NP);
	printf("sorted eigenvectors:\n");
	for (i=1;i<=NP;i++) {
		printf("eigenvalue %3d = %12.6f\n",i,d[i]);
		printf("eigenvector:\n");
		for (j=1;j<=NP;j++) {
			printf("%12.6f",v[j][i]);
			if ((j % 5) == 0) printf("\n");
		}
		printf("\n");
	}
	free_convert_matrix(e,1,NP,1,NP);
	free_matrix(v,1,NP,1,NP);
	free_vector(d,1,NP);
	return 0;
}
#undef NRANSI
