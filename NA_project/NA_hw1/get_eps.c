
#include<stdio.h>

void get_eps_f(int* n)
{
	float one = ((float)(1)); 
	float two = one + one;
	float zero = one - one;
	
	float tmp = one; //first set 1
	for (;;)
	{
		tmp = tmp / two; //divide with 2
		(*n)++;//count divide operation
		if (1 + tmp == 1) //if tmp becomes zero, break
			break;
	}
}
void get_eps_d(int* n)
{
	double one = ((double)(1));
	double two = one + one;
	double zero = one - one;

	double tmp = one;
	for (;;)
	{
		tmp = tmp / two;
		(*n)++;
		if (1 + tmp == 1)
			break;
	}
}

int main(void)
{
	int n1=0, n2 = 0;
	
	get_eps_f(&n1);
	get_eps_d(&n2);
	
	printf("float : %d\n", n1);
	printf("double : %d\n", n2);

	return 0;
}
