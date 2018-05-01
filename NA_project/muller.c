#include<stdio.h>
#include<math.h>
#define I 2
#pragma warning(disable : 4996)

float bessj0(float x)
{
	float ax, z;
	double xx, y, ans, ans1, ans2;

	if ((ax = fabs(x)) < 8.0) {
		y = x*x;
		ans1 = 57568490574.0 + y*(-13362590354.0 + y*(651619640.7
			+ y*(-11214424.18 + y*(77392.33017 + y*(-184.9052456)))));
		ans2 = 57568490411.0 + y*(1029532985.0 + y*(9494680.718
			+ y*(59272.64853 + y*(267.8532712 + y*1.0))));
		ans = ans1 / ans2;
	}
	else {
		z = 8.0 / ax;
		y = z*z;
		xx = ax - 0.785398164;
		ans1 = 1.0 + y*(-0.1098628627e-2 + y*(0.2734510407e-4
			+ y*(-0.2073370639e-5 + y*0.2093887211e-6)));
		ans2 = -0.1562499995e-1 + y*(0.1430488765e-3
			+ y*(-0.6911147651e-5 + y*(0.7621095161e-6
			- y*0.934945152e-7)));
		ans = sqrt(0.636619772 / ax)*(cos(xx)*ans1 - z*sin(xx)*ans2);
	}
	return ans;
}

int main()
{
	int i, itr, maxmitr;
	float x[4], li, di, mu, s, l, allerr;
	printf("\nEnter the three initial guesses:\n");
	for (i = I - 2; i < 3; i++)
	{
		scanf("%f", &x[i]);
	}
	for (i = I - 2; i < 3; i++)
	{
		printf("%f ", bessj0(x[i]));
	}
	printf("\n");
	printf("Enter allowed error and maximum iterations:\n");
	scanf("%f %d", &allerr, &maxmitr);
	for (itr = 1; itr <= maxmitr; itr++)
	{
		li = (x[I] - x[I - 1]) / (x[I - 1] - x[I - 2]);
		di = (x[I] - x[I - 2]) / (x[I - 1] - x[I - 2]);
		mu = bessj0(x[I - 2])*li*li - bessj0(x[I - 1])*di*di + bessj0(x[I])*(di + li);
		s = sqrt((mu*mu - 4 * bessj0(x[I])*di*li*(bessj0(x[I - 2])*li - bessj0(x[I - 1])*di + bessj0(x[I]))));
		if (mu < 0)
			l = (2 * bessj0(x[I])*di) / (-mu + s);
		else
			l = (2 * bessj0(x[I])*di) / (-mu - s);
		x[I + 1] = x[I] + l*(x[I] - x[I - 1]);
		printf("At iteration no. %3d, x = %7.5f\n", itr, x[I + 1]);
		if (fabs(x[I + 1] - x[I]) < allerr)
		{
			printf("After %3d iterations, the required root is %6.4f\n", itr, x[I + 1]);
			return 0;
		}
		for (i = I - 2; i<3; i++)
			x[i] = x[i + 1];
	}
	printf("The required solution does not converge or iterations are insufficient\n");
	return 1;
}