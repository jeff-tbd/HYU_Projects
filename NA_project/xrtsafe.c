
/* Driver for routine rtsafe */

#include <stdio.h>

#define M_E 2.71828182845904523536
#define PI 3.1415926535
#define NRANSI
#include "nr.h"
#include "nrutil.h"

#define N 100
#define NBMAX 20
#define X1 1.0
#define X2 10.0

float one(float x)
{
	double ans1 = 10 * exp(-x) * sin(2 * PI *x) - 2;
	//printf("one의 결과는 %f\n", f);
	return ans1;
}
float oned(float x)
{
	double ans1 = -10 * exp(-x) * sin(2 * PI *x) + 20*PI*exp(-x)*cos(2*PI*x);
	//printf("one의 결과는 %f\n", f);
	return ans1;
}

void funcdone(float x, float *fn, float *df)
{
	*fn = one(x);
	*df = oned(x);
}
static float fx1(float x)
{
	return bessj0(x);
}

static void funcd1(float x,float *fn, float *df)
{
	*fn=bessj0(x);
	*df = -bessj1(x);
}

static float fx2(float x)
{
	return bessj1(x);
}



static void funcd2(float x, float *fn, float *df)
{
	*fn = bessj1(x);
	*df = bessj0(x);
}


int main(void)
{
	int i,nb=NBMAX;
	float xacc,root,*xb1,*xb2;

	xb1=vector(1,NBMAX);
	xb2=vector(1,NBMAX);
	////////////////////////////
	zbrak(fx1, X1, X2, N, xb1, xb2, &nb);
	printf("brackets for roots of bessj0:\n");
	printf("%21s %10s %16s %10s\n", "lower", "upper", "f(lower)", "f(upper)");
	for (i = 1; i <= nb; i++)
		printf("%s %2d  %10.4f %10.4f %3s %10.4f %10.4f\n",
		"  root ", i, xb1[i], xb2[i], " ",
		fx1(xb1[i]), fx1(xb2[i]));
	
	////////////////////////////
	zbrak(fx1, X1, X2, N, xb1, xb2, &nb);
	printf("Bisection method Roots of bessj0:\n");
	printf("%21s %15s\n", "x", "f(x)");
	for (i = 1; i <= nb; i++) {
		xacc = (1.0e-6)*(xb1[i] + xb2[i]) / 2.0;
		root = rtbis(fx1, xb1[i], xb2[i], xacc);
		printf("root %3d %14.6f %14.6f\n", i, root, fx1(root));
	}
	
	////////////////////////////
	zbrak(fx1, X1, X2, N, xb1, xb2, &nb);
	printf("Linear Interpolation method Roots of bessj0:\n");
	printf("%21s %15s\n", "x", "f(x)");
	for (i = 1; i <= nb; i++) {
		xacc = (1.0e-6)*(xb1[i] + xb2[i]) / 2.0;
		root = rtflsp(fx1, xb1[i], xb2[i], xacc);
		printf("root %3d %14.6f %14.6f\n", i, root, fx1(root));
	}
	
	////////////////////////////
	zbrak(fx1, X1, X2, N, xb1, xb2, &nb);
	printf("Secant method Roots of bessj0:\n");
	printf("%21s %15s\n", "x", "f(x)");
	for (i = 1; i <= nb; i++) {
		xacc = (1.0e-6)*(xb1[i] + xb2[i]) / 2.0;
		root = rtsec(fx1, xb1[i], xb2[i], xacc);
		printf("root %3d %14.6f %14.6f\n", i, root, fx1(root));
	}
	
	///////////////////////////
	zbrak(fx1, X1, X2, N, xb1, xb2, &nb);
	printf("Newton-Raphson method Roots of bessj0:\n");
	printf("%21s %15s\n", "x", "f(x)");
	for (i = 1; i <= nb; i++) {
		xacc = (1.0e-6)*(xb1[i] + xb2[i]) / 2.0;
		root = rtnewt(funcd1, xb1[i], xb2[i], xacc);
		printf("root %3d %14.6f %14.6f\n", i, root, fx1(root));
	}
	zbrak(fx2, X1, X2, N, xb1, xb2, &nb);
	printf("Newton-Raphson method Roots of bessj1:\n");
	printf("%21s %15s\n", "x", "f(x)");
	for (i = 1; i <= nb; i++) {
		xacc = (1.0e-6)*(xb1[i] + xb2[i]) / 2.0;
		root = rtnewt(funcd2, xb1[i], xb2[i], xacc);
		printf("root %3d %14.6f %14.6f\n", i, root, fx2(root));
	}
	///////////////////////////////////
	zbrak(fx1,X1,X2,N,xb1,xb2,&nb);
	printf("Newton with bracketing nRoots of bessj0:\n");
	printf("%21s %15s\n","x","f(x)");
	for (i=1;i<=nb;i++) {
		xacc=(1.0e-6)*(xb1[i]+xb2[i])/2.0;
		root=rtsafe(funcd1,xb1[i],xb2[i],xacc);
		printf("root %3d %14.6f %14.6f\n",i,root,fx1(root));
	}
	zbrak(fx2, X1, X2, N, xb1, xb2, &nb);
	printf("Newton with bracketing Roots of bessj1:\n");
	printf("%21s %15s\n", "x", "f(x)");
	for (i = 1; i <= nb; i++) {
		xacc = (1.0e-6)*(xb1[i] + xb2[i]) / 2.0;
		root = rtsafe(funcd2, xb1[i], xb2[i], xacc);
		printf("root %3d %14.6f %14.6f\n", i, root, fx2(root));
	}

	//////////////////////////////////////
	zbrak(one, 0.1, 1, N, xb1, xb2, &nb);
	printf("Newton with bracketing Roots of one:\n");
	printf("%21s %15s\n", "x", "f(x)");
	printf("nb value : %d\n", nb);
	for (i = 1; i <= nb; i++) {
		xacc = (1.0e-6)*(xb1[i] + xb2[i]) / 2.0;
		root = rtsafe(funcdone, xb1[i], xb2[i], xacc);
		printf("root is %f", root);
		printf("root %3d %14.6f %14.6f\n", i, root, one(root));
	}


	free_vector(xb2,1,NBMAX);
	free_vector(xb1,1,NBMAX);
	return 0;
}
#undef NRANSI
