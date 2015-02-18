#include <fcntl.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int outputFd, openFlags;
    char abc[] = "abc";

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);

    /* Open input and output files */

    openFlags = O_WRONLY | O_APPEND;
    outputFd = open(argv[1], openFlags);
    if (outputFd == -1)
        errExit("opening file %s", argv[2]);

    /* Transfer data until we encounter end of input or an error */

    if (lseek(outputFd, 0, SEEK_SET) == -1)
        fatal("could not seek the file");
    if (write(outputFd, abc, strlen(abc)) != strlen(abc))
        fatal("couldn't write abc");

    if (close(outputFd) == -1)
        errExit("close output");

    exit(EXIT_SUCCESS);
}
