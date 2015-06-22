#include "tlpi_hdr.h"
#include "read_line.h"
#include <sys/socket.h>

int mypipe(int[2]);

int mypipe(int fd[2]) {
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1)
        return -1;
    if (shutdown(fd[0], SHUT_WR) == -1 ||
        shutdown(fd[1], SHUT_RD) == -1)
        return -1;
    return 0;
}

#define BUF_SIZE 1024

int main(int argc, char const *argv[])
{
    int fd[2];
    ssize_t numRead;
    char buf[BUF_SIZE+1];
    if (mypipe(fd) == -1)
        errExit("mypipe error");
    switch (fork()) {
    case -1: errExit("fork error");
    case 0:
        for (;;) {
            numRead = readLine(STDIN_FILENO, buf, BUF_SIZE);
            if (numRead == -1)
                errExit("child: read error");
            if (write(fd[1], buf, numRead) == -1)
                errExit("child: write error");
        }
        break;
    default:
        for (;;) {
            numRead = read(fd[0], buf, BUF_SIZE);
            if (numRead == -1)
                errExit("parent: read error");
            if (write(STDOUT_FILENO, buf, numRead) == -1)
                errExit("parent: write error");
        }
        break;
    }
    return 0;
}
