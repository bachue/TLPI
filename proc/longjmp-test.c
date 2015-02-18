#include <setjmp.h>
#include "tlpi_hdr.h"

static jmp_buf env;

static void f1(int val)
{
    longjmp(env, val);
}

int
main(int argc, char *argv[])
{
    int val, ret;
    volatile int test;
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s val", argv[0]);

    val = atoi(argv[1]);
    test = 1;

    switch (ret = setjmp(env)) {
    case 0:
        test = 2;
        f1(val);                /* Never returns... */
        break;                  /* ... but this is good form */
    default:
        printf("We jumped back from f1(), returns %d, test = %d\n", ret, test);
        break;
    }

    exit(EXIT_SUCCESS);
}
