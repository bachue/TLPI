#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "tlpi_hdr.h"
#define STR_SIZE 100

int
main(int argc, char *argv[])
{
    int inputFd, outputFd;
    struct iovec iov[4];
    double f;                   /* First buffer */
    int n;                      /* Second buffer */
    long l;                     /* Third buffer */
    char str[STR_SIZE];         /* Fourth buffer */
    ssize_t numRead, numWrite, toRead, toWrite;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s buffer\n", argv[0]);

    outputFd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC);
    if (outputFd == -1)
        errExit("open for writing");

    f = 3.1415926;
    n = 1024;
    l = 4096;
    strncpy(str, "Hello bachue, this is a test example for readv and writev.", STR_SIZE);

    toWrite = 0;
    
    iov[0].iov_base = &f;
    iov[0].iov_len = sizeof(double);
    toWrite += iov[0].iov_len;

    iov[1].iov_base = &n;
    iov[1].iov_len = sizeof(int);
    toWrite += iov[1].iov_len;

    iov[2].iov_base = &l;
    iov[2].iov_len = sizeof(long);
    toWrite += iov[2].iov_len;

    iov[3].iov_base = str;
    iov[3].iov_len = sizeof(str);
    toWrite += iov[3].iov_len;

    numWrite = writev(outputFd, iov, 4);
    if (numWrite == -1)
        errExit("writev");
    if (numWrite < toWrite)
        errExit("Write fewer bytes than requested");

    memset(iov, 0, sizeof(struct iovec) * 4);

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1)
        errExit("open for reading");

    toRead = 0;

    iov[0].iov_base = &f;
    iov[0].iov_len = sizeof(double);
    toRead += iov[0].iov_len;

    iov[1].iov_base = &n;
    iov[1].iov_len = sizeof(int);
    toRead += iov[1].iov_len;

    iov[2].iov_base = &l;
    iov[2].iov_len = sizeof(long);
    toRead += iov[2].iov_len;

    iov[3].iov_base = str;
    iov[3].iov_len = sizeof(str);
    toRead += iov[3].iov_len;

    numRead = readv(inputFd, iov, 4);
    if (numRead == -1)
        errExit("readv");

    if (numRead < toRead)
        errExit("Read fewer bytes than requested");

    printf("double: %f, int: %d, long: %ld, str: %s",
            f, n, l, str);
    exit(EXIT_SUCCESS);
}
