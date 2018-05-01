
/* Driver for routine gasdev */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define NRANSI
#include "nr.h"

#define N 100
#define NPTS 10000
#define ISCAL 400
#define LLEN 60

int main(void)
{
	char words[LLEN+1];
	int i,j,k,klim,dist[N+1];
	float dd,x;

	srand(time(NULL));

	for (j=0;j<=N;j++) dist[j]=0;
	for (i=1;i<=NPTS;i++) {
		x = gasdev(0.5, 1.25);
		for (k = 0; k <= N; k++)
		{
			if ((-2+k*0.05) <= x && (-2+k*0.1) >= x)
			{
				dist[k] += 1;
			}
		}
	}
	printf("uniform distributed deviate of %6d points\n",NPTS);
	printf ("%5s %10s %9s\n","x","p(x)","graph:");
	for (j=0;j<=N;j++) {
		for (k=1;k<=LLEN;k++) words[k]=' ';
		for (k=0;k<(dist[j]/150);k++) words[k]='*';
		printf("%8.4f %f  ",-2.0+j*0.05,(float)dist[j]/1000.0);
		for (k=1;k<=LLEN;k++) printf("%c",words[k]);
		printf("\n");
	}
	return 0;
}
#undef NRANSI
