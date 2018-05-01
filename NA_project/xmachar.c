/* Driver for routine machar */

#include <stdio.h>
#define NRANSI
#include "nr.h"

int main(void)
{
	int ibeta,iexp,irnd,it,machep,maxexp,minexp,negep,ngrd;
	float eps,epsneg,xmax,xmin;
	double deps, depsneg, dxmax, dxmin;

	machar(&ibeta,&it,&irnd,&ngrd,&machep,&negep,&iexp,&minexp,&maxexp,
		&eps,&epsneg,&xmin,&xmax);
	printf("<float case>--------------------------------------\n");
	printf("ibeta = %d\n",ibeta);
	printf("it = %d\n",it);
	printf("irnd = %d\n",irnd);
	printf("ngrd = %d\n",ngrd);
	printf("machep = %d\n",machep);
	printf("negep = %d\n",negep);
	printf("iexp = %d\n",iexp);
	printf("minexp = %d\n",minexp);
	printf("maxexp = %d\n",maxexp);
	printf("eps = %12.6g\n",eps);
	printf("epsneg = %12.6g\n",epsneg);
	printf("xmin = %12.6g\n",xmin);
	printf("xmax = %12.6g\n",xmax);
	
	///////////////////double
	dmachar(&ibeta, &it, &irnd, &ngrd, &machep, &negep, &iexp, &minexp, &maxexp,
		&deps, &depsneg, &dxmin, &dxmax);
	printf("<double case>--------------------------------------\n");
	printf("ibeta = %d\n", ibeta);
	printf("it = %d\n", it);
	printf("irnd = %d\n", irnd);
	printf("ngrd = %d\n", ngrd);
	printf("machep = %d\n", machep);
	printf("negep = %d\n", negep);
	printf("iexp = %d\n", iexp);
	printf("minexp = %d\n", minexp);
	printf("maxexp = %d\n", maxexp);
	printf("eps = %12.6g\n", deps);
	printf("epsneg = %12.6g\n", depsneg);
	printf("xmin = %12.6g\n", dxmin);
	printf("xmax = %12.6g\n", dxmax);

	system("pause");
	return 0;
}
#undef NRANSI
