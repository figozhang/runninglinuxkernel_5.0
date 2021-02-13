#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 4096
#define MAX_SIZE 100*PAGE_SIZE

int main()
{
    char *buf = (char *)malloc(MAX_SIZE);

    memset(buf, 0, MAX_SIZE);

    printf("buffer address=0x%p\n", buf);

    free(buf);
           return 0;
}
