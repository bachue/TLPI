#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    int srcfd, dstfd, pipefd[2];
    struct stat st;
    if (argc != 3)
        usageErr("%s src dst\n", argv[0]);

    srcfd = open(argv[1], O_RDONLY);
    if (srcfd == -1)
        errExit("open error");

    dstfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
    if (dstfd == -1)
        errExit("open error");

    if (fstat(srcfd, &st) == -1)
        errExit("fstat error");

    if (pipe(pipefd) == -1)
        errExit("pipe error");

    if (splice(srcfd, NULL, pipefd[1], NULL, st.st_size, SPLICE_F_MOVE) == -1)
        errExit("splice 1 error");

    if (splice(pipefd[0], NULL, dstfd, NULL, st.st_size, SPLICE_F_MOVE) == -1)
        errExit("splice 2 error");

    return 0;
}
