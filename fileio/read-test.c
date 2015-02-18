#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 10
#endif

int
main(int argc, char *argv[])
{
    int inputFd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1)
        errExit("opening file %s", argv[1]);

    while ((numRead = read(inputFd, buf, BUF_SIZE-1)) > 0) {
buf[BUF_SIZE-1]=0;
        printf("%s|", buf);
    }

    if (numRead == -1)
        errExit("read");

    if (close(inputFd) == -1)
        errExit("close input");

    exit(EXIT_SUCCESS);
}
