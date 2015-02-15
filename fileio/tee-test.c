#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif

void usage(char *);

int
main(int argc, char *argv[])
{
    int outputFd, openFlags, i, append = 0;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];
    char *output = NULL;

    if (argc < 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-a") == 0)
            append = 1;
        else if (output == NULL)
            output = argv[i];
        else {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    openFlags = O_CREAT | O_WRONLY;
    if (append)
        openFlags |= O_APPEND;
    else
        openFlags |= O_TRUNC;

    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- */
    outputFd = open(output, openFlags, filePerms);
    if (outputFd == -1)
        errExit("opening file %s", argv[2]);

    /* Transfer data until we encounter end of input or an error */

    while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, numRead) != numRead)
            fatal("couldn't write whole buffer");
        if (write(STDOUT_FILENO, buf, numRead) != numRead)
            fatal("couldn't write whole buffer");
    }
    if (numRead == -1)
        errExit("read");

    if (close(outputFd) == -1)
        errExit("close output");

    exit(EXIT_SUCCESS);
}

void usage(char *program) {
    printf("Usage: %s [-a] FILE\n", program);
}
