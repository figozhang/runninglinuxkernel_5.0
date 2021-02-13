#include <stdio.h>

extern int compare_data(int a, int b);

int main()
{
	int val;

	val = compare_data(5, 6);

	printf("big data: %d\n", val);
}
