#include <sys/file.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(int argc, char const *argv[])
{
    int fd, status;
    struct flock fl;
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);

    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        errExit("open fd error");

    switch (fork()) {
    case -1: errExit("fork error");
    case 0:
        if (flock(fd, LOCK_EX) == -1)
            errExit("lock fd1 error");
        printf("flock in child\n");
        if (fflush(stdout) != 0)
            errExit("fflush error in child");
        sleep(5);
        _exit(EXIT_SUCCESS);
    default:
        fl.l_start = fl.l_len = 0;
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        status = fcntl(fd, F_SETLK, &fl);           /* Perform request... */
        if (status == 0) {
            printf("fcntl lock in parent\n");
            if (fflush(stdout) != 0)
                errExit("fflush error in parent");
            sleep(5);
        } else if (errno == EAGAIN || errno == EACCES)
            errExit("failed to lock file in parent");
        else if (errno == EDEADLK)
            errExit("failed to lock file due to dead lock in parent");
        else
            errExit("failed to lock file in parent");
        break;
    }

    return 0;
}
