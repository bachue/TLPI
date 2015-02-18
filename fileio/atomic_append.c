#include <fcntl.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int outputFd, openFlags, numBytes, toAppend = 0, i;
    char *filename, byte;

    if ((argc != 4 && argc != 5) || strcmp(argv[1], "--help") == 0)
        usageErr("%s filename num-bytes byte [x]\n", argv[0]);
    openFlags = O_WRONLY;
    filename = argv[1];
    numBytes = atoi(argv[2]);
    byte = argv[3][0];
    toAppend = argv[4] != NULL;

    if (toAppend)
        openFlags |= O_APPEND;

    outputFd = open(filename, openFlags);
    if (outputFd == -1)
        errExit("opening file %s", filename);

    for (i = 0; i < numBytes; ++i) {
        if (!toAppend)
            if (lseek(outputFd, 0, SEEK_END) == -1)
                errExit("seeking file %s", filename);
        if (write(outputFd, &byte, 1) != 1)
            fatal("couldn't write whole buffer");
    }

    if (close(outputFd) == -1)
        errExit("close output");

    exit(EXIT_SUCCESS);
}
