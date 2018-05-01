
#include <math.h>
#include <time.h>
#include <stdlib.h>

float gasdev(float average, float stdev)
{
	float gset;
	float fac,rsq,v1,v2;
	

	do {
		v1 = 5.0*((float)rand()/RAND_MAX)-2.0;
		//v2 = 5.0*((float)rand()/RAND_MAX)-1.0;
		//rsq=v1*v1+v2*v2;
	} while (v1 > 3.0 || v1 < -2.0);
	/*
	fac=sqrt(-2.0*log(rsq)/rsq);
	gset=v1*fac;
	gset = (stdev*gset) + average;
	return gset;*/
	return v1;
}
