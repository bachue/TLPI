#include <sys/file.h>
#include <fcntl.h>
#include <sys/types.h>
#include "tlpi_hdr.h"

int main(int argc, char const *argv[])
{
    int fd, i, status;
    struct flock fl;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);

    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        errExit("open fd error");

    if (ftruncate(fd, 80000) == -1)
        errExit("ftruncate error");

    for (i = 0; i <= 80000; i += 2) {
        fl.l_start = i;
        fl.l_len = 1;
        fl.l_whence = SEEK_SET;
        fl.l_type = F_WRLCK;
        status = fcntl(fd, F_SETLK, &fl);
        if (status == 0) {
            printf("Lock: %d\n", i);
        }
        else {
            switch (errno) {
            case EAGAIN:
            case EACCES:
                errExit("failed (incompatible lock)");
            case EDEADLK:
                errExit("failed (deadlock)");
            default:
                errExit("failed (unknown)");
            }
        }
    }

    printf("success\n");
    pause();

    return 0;
}
