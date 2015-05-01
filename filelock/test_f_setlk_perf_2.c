#include <sys/file.h>
#include <fcntl.h>
#include <sys/types.h>
#include "tlpi_hdr.h"

int main(int argc, char const *argv[])
{
    int fd, i, num, status;
    struct flock fl;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file num\n", argv[0]);

    num = getInt(argv[2], GN_NONNEG, "num");

    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        errExit("open fd error");

    fl.l_len = 1;
    fl.l_whence = SEEK_SET;
    fl.l_type = F_WRLCK;
    for (i = 0; i < 10000; ++i) {
        fl.l_start = num * 2;
        status = fcntl(fd, F_SETLK, &fl);
        if (status == 0) {
            fatal("Got lock, impossible!");
            exit(EXIT_FAILURE);
        } else {
            switch (errno) {
            case EAGAIN:
                errno = 0;
                break;
            case EDEADLK:
                errExit("failed (deadlock)");
            default:
                errExit("failed (unknown)");
            }
        }
    }

    printf("success\n");
    return 0;
}
