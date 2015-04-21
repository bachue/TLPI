#include "test_memory_share.h"

int main(int argc, char const *argv[])
{
    int reqfd, respfd, sharefd;
    long xfrs, bytes;
    char *buf;
    size_t writenum;

    reqfd = open(REQ_PATH, O_RDONLY | O_CLOEXEC);
    if (reqfd == -1)
        errExit("open req fifo error");

    respfd = open(RESP_PATH, O_WRONLY | O_CLOEXEC);
    if (respfd == -1)
        errExit("open resp fifo error");

    sharefd = open(SHARED_MEM_PATH, O_RDWR);
    if (sharefd == -1)
        errExit("open shared file error");

    buf = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharefd, 0);
    if (buf == MAP_FAILED)
        errExit("mmap error");

    for (xfrs = bytes = 0; ; xfrs++) {
        if (read(reqfd, &writenum, sizeof(writenum)) == -1)
            errExit("read from req fifo error");
        if (writenum == 0) break;
        bytes += writenum;
        if (write(STDOUT_FILENO, buf, writenum) == -1)
            errExit("write to STDOUT error");
        if (write(respfd, buf, 1) == -1)
            errExit("write response to resp fifo error");
    }
    fprintf(stderr, "Received %ld bytes (%ld xfrs)\n", bytes, xfrs);

    if (munmap(buf, BUF_SIZE) == -1)
        errExit("mummap error");

    if (close(sharefd) == -1)
        errExit("close shared file error");
    if (close(reqfd) == -1)
        errExit("close req fifo error");
    if (close(respfd) == -1)
        errExit("close resp fifo error");
    return 0;
}
