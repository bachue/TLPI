#include "tlpi_hdr.h"
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 1024

int main(int argc, char const *argv[])
{
    int filed, fd[2], numRead;
    char buf[BUF_SIZE];
    struct stat st;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s path\n", argv[0]);

    filed = open(argv[1], O_RDONLY);
    if (filed == -1)
        errExit("open error");

    if (fstat(filed, &st) == -1)
        errExit("fstat error");

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1)
        errExit("socketpair error");
    switch (fork()) {
    case -1: errExit("fork error");
    case 0:
        for (;;) {
            numRead = read(fd[0], buf, BUF_SIZE);
            if (numRead == -1)
                errExit("read error");
            if (numRead == 0)
                break;
            if (write(STDOUT_FILENO, buf, numRead) == -1)
                errExit("write error");
        }
        break;
    default:
        if (sendfile(fd[1], filed, NULL, st.st_size) == -1)
            errExit("sendfile error");
        break;
    }
    return 0;
}
