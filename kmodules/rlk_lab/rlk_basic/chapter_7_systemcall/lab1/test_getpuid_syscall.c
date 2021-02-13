#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
	long pid, uid;
	int ret;

	ret = (int)syscall(295, &pid, &uid);
	if (ret != 0) {
		printf("call getpuid failed\n");
		return 1;
	}

	printf("call getpuid success, return pid = %d, uid = %d\n", pid, uid);

	return 0;
}
