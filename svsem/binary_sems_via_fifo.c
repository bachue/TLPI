#include <sys/types.h>
#include "binary_sems_via_fifo.h"
#include <fcntl.h>

Boolean bsUseSemUndo = FALSE;
Boolean bsRetryOnEintr = TRUE;

int initSemAvailable(char *path) {
    int fd;
    fd = open(path, O_WRONLY);
    if (write(fd, '\0', 1) == -1)
        return -1;
    return close(fd);
}

int initSemInUse(char *path) {
    return 0;
}

/* Reserve semaphore (blocking), return 0 on success, or -1 with 'errno'
   set to EINTR if operation was interrupted by a signal handler */

int reserveSem(char *path) {
    int fd;
    char buf;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        return -1;
    if (read(fd, &buf, 1) == -1)
        return -1;
    return close(fd);
}

int releaseSem(char *path) {
    int fd;
    char buf = '\0';
    fd = open(path, O_WRONLY);
    if (fd == -1)
        return -1;
    if (write(fd, &buf, 1) == -1)
        return -1;
    return close(fd);
}
