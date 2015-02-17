#include <fcntl.h>
#include "tlpi_hdr.h"

int mydup(int fd);
int mydup2(int fd, int fd2);

int
main(int argc, char *argv[])
{
    int fd, fd2;
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file case", argv[0]);

    fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC);
    if (fd == -1)
        errExit("opening file %s", argv[1]);

    if (strcmp(argv[2], "1") == 0) {
        fd2 = mydup(fd);
        if (fd2 == -1) errExit("mydup failed");
        if (fd == fd2) fatal("mydup cannot make fd equal than fd2");
        if (write(fd, "abc", 3) != 3 || write(fd2, "def", 3) != 3)
            errExit("failed to write file");
    } else if (strcmp(argv[2], "2") == 0) {
        fd2 = STDERR_FILENO;
        if (mydup2(fd, fd2) != fd2) errExit("mydup2 failed");
        if (write(fd, "abc", 3) != 3 || write(fd2, "def", 3) != 3)
            errExit("failed to write file");
        fprintf(stderr, "%s\n", "ghi");
    } else if (strcmp(argv[2], "3") == 0) {
        int fd3;
        fd2 = mydup(fd);
        fd3 = open(argv[1], O_WRONLY);
        if (fd2 == -1) errExit("mydup failed");
        if (fd3 == -1) errExit("opening file secondly %s", argv[1]);

        if (write(fd, "Hello,", 6) == -1 ||
            write(fd2, "world", 5) == -1 ||
            lseek(fd2, 0, SEEK_SET) == -1 ||
            write(fd, "Hello,", 6) == -1 ||
            write(fd3, "Gidday", 6) == -1)
            errExit("failed to execute test case");
    } else {
        fatal("Unrealized case");
    }

    if (close(fd) == -1)
        errExit("closing file %s", argv[1]);
    if (close(fd2) == -1)
        errExit("closing dupped", argv[1]);

    exit(EXIT_SUCCESS);
}

int mydup(int fd) {
    return fcntl(fd, F_DUPFD, 0);
}

int mydup2(int fd, int fd2) {
    if (fd == fd2) {
        if (fcntl(fd, F_GETFL) == -1) {
            errno = EBADF;
            return -1;
        } else {
            return fd2;
        }
    }

    close(fd2);
    return fcntl(fd, F_DUPFD, fd2);
}
