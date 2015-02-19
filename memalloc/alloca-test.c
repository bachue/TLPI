#include "tlpi_hdr.h"

#define SIZE 100

static void copy_and_read(char *ptr) {
    strncpy(ptr, "Hello world", SIZE);
    printf("1: %s\n", ptr);
}

int
main(int argc, char *argv[])
{
    char *ptr;

    ptr = (char *) alloca(SIZE);
    if (ptr == NULL)
        fatal("Failed to allocate memory");

    copy_and_read(ptr);
    printf("2: %s\n", ptr);

    exit(EXIT_SUCCESS);
}
