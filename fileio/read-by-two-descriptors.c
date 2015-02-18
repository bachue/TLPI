#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 10
#endif

int
main(int argc, char *argv[])
{
    int inputFd1, inputFd2, inputFd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    /* Open input and output files */

    inputFd1 = open(argv[1], O_RDONLY);
    if (inputFd1 == -1)
        errExit("opening file %s", argv[1]);

    inputFd2 = dup(inputFd1);
    if (inputFd2 == -1)
        errExit("dup file %s", argv[1]);

    inputFd = inputFd1;
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        inputFd = inputFd == inputFd1 ? inputFd2 : inputFd1;
        if (write(STDOUT_FILENO, buf, numRead) != numRead)
            fatal("couldn't write whole buffer into stdout");
    }
    if (numRead == -1)
        errExit("read");

    if (close(inputFd1) == -1)
        errExit("close input");
    if (close(inputFd2) == -1)
        errExit("close output");

    exit(EXIT_SUCCESS);
}
