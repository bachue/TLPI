#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    int srcfd, dstfd, teefd, pipefd[2], teepipefd[2];
    struct stat st;
    if (argc != 4)
        usageErr("%s src dst\n", argv[0]);

    srcfd = open(argv[1], O_RDONLY);
    if (srcfd == -1)
        errExit("open error");

    dstfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
    if (dstfd == -1)
        errExit("open error");

    teefd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC);
    if (teefd == -1)
        errExit("open error");

    if (fstat(srcfd, &st) == -1)
        errExit("fstat error");

    if (pipe(pipefd) == -1)
        errExit("pipe error");

    if (pipe(teepipefd) == -1)
        errExit("pipe error");

    if (splice(srcfd, NULL, pipefd[1], NULL, st.st_size, SPLICE_F_MOVE) == -1)
        errExit("splice 1 error");

    if (tee(pipefd[0], teepipefd[1], st.st_size, 0) == -1)
        errExit("tee error");

    if (splice(pipefd[0], NULL, dstfd, NULL, st.st_size, SPLICE_F_MOVE) == -1)
        errExit("splice 2 error");

    if (splice(teepipefd[0], NULL, teefd, NULL, st.st_size, SPLICE_F_MOVE) == -1)
        errExit("splice 3 error");

    return 0;
}
