
/* Driver for routine ludcmp */

#include <stdio.h>
#include <stdlib.h>
#define NRANSI
#include "nr.h"
#include "nrutil.h"

#define NP 20
#define MAXSTR 80

int main(void)
{
	int j,k,l,n,dum,*indx,*jndx;
	int m = 1;
	float d,**a,**xl,**xu,**x, **ai;
	float *col, *sol, **aa, *b;
	//char dummy[MAXSTR];
	FILE *fp;

	indx=ivector(1,NP);
	jndx=ivector(1,NP);
	b = vector(1, NP);
	sol = vector(1, NP);
	col = vector(1, NP);
	a=matrix(1,NP,1,NP);
	aa = matrix(1, NP, 1, NP);
	xl=matrix(1,NP,1,NP);
	xu=matrix(1,NP,1,NP);
	x=matrix(1,NP,1,NP);
	ai = matrix(1, NP, 1, NP);
	fopen_s(&fp, "lineq2.dat", "r");
	if (fp == NULL)
		nrerror("Data file is not found\n");
	while (!feof(fp)) {
		//fgets(dummy,MAXSTR,fp);
		//fgets(dummy,MAXSTR,fp);
		fscanf_s(fp,"%d %d ",&n,&n);
		//fgets(dummy,MAXSTR,fp);
		for (k=1;k<=n;k++)
			for (l=1;l<=n;l++) fscanf_s(fp,"%f ",&a[k][l]);
		//fgets(dummy,MAXSTR,fp);
		for (l=1;l<=m;l++)
			for (k=1;k<=n;k++) fscanf_s(fp,"%f ",&x[k][l]);
		/* Print out a-matrix for comparison with product of
		   lower and upper decomposition matrices */
		printf("original matrix:\n");
		for (k=1;k<=n;k++) {
			for (l=1;l<=n;l++) printf("%12.6f",a[k][l]);
			printf("\n");
		}
		/*save initial values*/
		for (l = 1; l <= n; l++) {
			b[l] = x[l][1];
			for (j = 1; j <= n; j++)
				aa[l][j] = a[l][j];
		}
		/* Perform the decomposition */
		ludcmp(a,n,indx,&d);
		/*calculating the determinant*/
		for (j = 1; j <= n; j++){
			d *= a[j][j];
		}
		printf("determinant is %f\n", d);
		/* Compose separately the lower and upper matrices */
		for (k=1;k<=n;k++) {
			for (l=1;l<=n;l++) {
				if (l > k) {
					xu[k][l]=a[k][l];
					xl[k][l]=0.0;
				} else if (l < k) {
					xu[k][l]=0.0;
					xl[k][l]=a[k][l];
				} else {
					xu[k][l]=a[k][l];
					xl[k][l]=1.0;
				}
			}
		}
		/* Compute product of lower and upper matrices for
		   comparison with original matrix */
		for (k=1;k<=n;k++) {
			jndx[k]=k;
			for (l=1;l<=n;l++) {
				x[k][l]=0.0;
				for (j=1;j<=n;j++)
					x[k][l] += (xl[k][j]*xu[j][l]);
			}
		}
		printf("\n%s%s\n","product of lower and upper ",
			 "matrices (rows unscrambled):");
		for (k=1;k<=n;k++) {
			dum=jndx[indx[k]];
			jndx[indx[k]]=jndx[k];
			jndx[k]=dum;
		}
		for (k=1;k<=n;k++)
			for (j=1;j<=n;j++)
				if (jndx[j] == k) {
					for (l=1;l<=n;l++)
						printf("%12.6f",x[j][l]);
					printf("\n");
				}
		printf("\nlower matrix of the decomposition:\n");
		for (k=1;k<=n;k++) {
			for (l=1;l<=n;l++) printf("%12.6f",xl[k][l]);
			printf("\n");
		}
		printf("\nupper matrix of the decomposition:\n");
		for (k=1;k<=n;k++) {
			for (l=1;l<=n;l++) printf("%12.6f",xu[k][l]);
			printf("\n");
		}
		printf("\inverse matrix by using LU decomposition:\n");
		for (k = 1; k <= n; k++) {
			for (l = 1; l <= n; l++) {
				col[l] = 0.0;
			}
			b[k] = col[k] = 1.0;
			lubksb(a, n, indx, col);
			//mprove(aa, a, n, indx, b, col);
			for (l = 1; l <= n; l++){
				ai[l][k] = col[l];
				printf("%12.6f\t", ai[l][k]);
			}
			printf("\n");
		}
		
		
		//////
		printf("\nsolving the equation\n");
		for (l = 1; l <= n; l++){
			sol[l] = 0.0;
			for (j = 1; j <= n; j++){
				sol[l] += ai[l][j] * x[j][l];
			}
			printf("%12.6f\n", sol[l]);
		}
		
		printf("\n***********************************\n");
		printf("press return for next problem:\n");
		(void) getchar();
	}
	fclose(fp);
	free_matrix(x,1,NP,1,NP);
	free_matrix(xu,1,NP,1,NP);
	free_matrix(xl,1,NP,1,NP);
	free_matrix(a,1,NP,1,NP);
	free_ivector(jndx,1,NP);
	free_ivector(indx,1,NP);
	return 0;
}
#undef NRANSI
