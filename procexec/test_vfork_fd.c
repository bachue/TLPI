#include "tlpi_hdr.h"
#include <fcntl.h>

int main(int argc, char  *argv[]) {
    int fd;
    char template[] = "/tmp/vforktest.XXXXXX";

    fd = mkstemp(template);
    if (fd == -1)
        errExit("mkstemp error");
    switch (vfork()) {
        case -1: errExit("vfork error");
        case 0:
            if (write(fd, "written by child\n", strlen("written by child\n")) == -1)
                errExit("Child: write error");
            if (close(fd) == -1)
                errExit("Child: close error");
            _exit(EXIT_SUCCESS);
        default:
            if (write(fd, "written by parent\n", strlen("written by parent\n")) == -1)
                errExit("Parent: close error");
            if (close(fd) == -1)
                errExit("Child: close error");
            exit(EXIT_SUCCESS);
    }
}
