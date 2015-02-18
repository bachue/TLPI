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
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s val", argv[0]);

    val = atoi(argv[1]);

    switch (ret = setjmp(env)) {
    case 0:
        f1(val);                /* Never returns... */
        break;                  /* ... but this is good form */
    default:
        printf("We jumped back from f1(), returns %d\n", ret);
        break;
    }

    exit(EXIT_SUCCESS);
}
