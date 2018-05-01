
#include <math.h>
#include <time.h>
#include <stdlib.h>

float gasdev(float average, float stdev)
{
	float gset;
	float fac,rsq,v1,v2;
	

	do {
		v1 = 2.0*((float)rand()/RAND_MAX)-1.0;
		v2 = 2.0*((float)rand()/RAND_MAX)-1.0;
		rsq=v1*v1+v2*v2;
	} while (rsq >= 1.0 || rsq <= -1.0);
	
	fac=sqrt(-2.0*log(rsq)/rsq);
	gset=v1*fac;
	gset = (stdev*gset) + average;
	return gset;
}
