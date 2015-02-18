#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int result;
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s FILE\n", argv[0]);

    /* Open input and output files */

    result = truncate(argv[1], 0);
    if (result == -1)
        errExit("truncating file %s", argv[1]);

    exit(EXIT_SUCCESS);
}
