#include <stdio.h>

static int compare_data(int a, int b)
{
	int val;

	asm volatile (
			"cmp %1, %2\n"
			"csel %0, %1, %2, hi\n"
			: "+r" (val)
			: "r" (a), "r" (b)
			: "memory");

	return val;
}

int main()
{
	int val;

	val = compare_data(5, 6);
	printf("big data: %d\n", val);

	val = compare_data(6, 4);
	printf("big data: %d\n", val);
}

