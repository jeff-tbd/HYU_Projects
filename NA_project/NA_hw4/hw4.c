
/* Driver for routine rtbis */

#include <stdio.h>
#include <math.h>
#define NRANSI

#include "nr.h"
#include "nrutil.h"

#define N 100
#define NBMAX 20
#define X1 0.0
#define X2 400.0
/*
static float fx1(float x)
{
	return bessj0(x);
}
static float fx2(float x)
{
	return bessj1(x);
}*/
static float func(float r)
{
	return exp(-0.005*r)*cos(floor(sqrt(2000.0 - 0.01*r*r)*0.05)) - 0.01;
}


int main(void)
{
	int i,nb=NBMAX;
	float xacc, root, *xb1, *xb2;
	//printf("%f ¬Î¿Ã 0¿Œ∞°", func(204));
	xb1=vector(1,NBMAX);
	xb2 = vector(1, NBMAX);
	zbrak(func, X1, X2, N, xb1, xb2, &nb);
	printf("\nRoots of bessj0:\n");
	printf("%21s %15s\n", "x", "f(x)");
	for (i = 1; i <= nb; i++) {
		xacc = (1.0e-6)*(xb1[i] + xb2[i]) / 2.0;
		root = rtbis(func, xb1[i], xb2[i], xacc);
		printf("bracket method case\n");
		printf("root %3d %14.6f %14.6f\n", i, root, func(root));
	}
	for (i = 1; i <= nb; i++) {
		xacc = (1.0e-6)*(xb1[i] + xb2[i]) / 2.0;
		root = rtflsp(func, xb1[i], xb2[i], xacc);
		printf("linear method case\n");
		printf("root %3d %14.6f %14.6f\n", i, root, func(root));
	}
	/*
	xb2=vector(1,NBMAX);
	zbrak(fx1,X1,X2,N,xb1,xb2,&nb);
	printf("\nRoots of bessj0:\n");
	printf("%21s %15s\n","x","f(x)");
	for (i=1;i<=nb;i++) {
		xacc=(1.0e-6)*(xb1[i]+xb2[i])/2.0;
		root=rtbis(fx1,xb1[i],xb2[i],xacc);
		printf("root %3d %14.6f %14.6f\n",i,root,fx1(root));
	}
	zbrak(fx2, X1, X2, N, xb1, xb2, &nb);
	printf("\nRoots of bessj1:\n");
	printf("%21s %15s\n", "x", "f(x)");
	for (i = 1; i <= nb; i++) {
		xacc = (1.0e-6)*(xb1[i] + xb2[i]) / 2.0;
		root = rtbis(fx2, xb1[i], xb2[i], xacc);
		printf("root %3d %14.6f %14.6f\n", i, root, fx2(root));
	}*/

	free_vector(xb2,1,NBMAX);
	free_vector(xb1,1,NBMAX);
	
	return 0;
}
#undef NRANSI
