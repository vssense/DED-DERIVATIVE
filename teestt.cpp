#include <stdio.h>
#include <stdlib.h>

int main()
{
	const char* str = "(123321)";

	double a = 7.5;
	int b = 2;
	printf("%lf\n", a%b);

	// int n = 0;
	// int n1 = 0;
	// int n2 = 0;
	// int i = 1;
	// sscanf(str + i, "%n%d%n", &n1, &n, &n2);
	// printf("%d\n", n);
	// i += n2 - n1;
	// printf("%d\n", n2 - n1);
	// printf("%c\n", str[i]);
	return 0;
}