#include "tlpi_hdr.h"
#include <stdio.h>

int
main(int argc, char *argv[])
{
    off_t off;

    off = 1024;

    printf("sizeof(offset_t)=%lu, offset=%ld\n", sizeof(off_t), (long) off);

    exit(EXIT_SUCCESS);
}
