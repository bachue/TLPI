#include "binary_sems_via_fifo.h"
#include "curr_time.h"                  /* Declaration of currTime() */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/fifo"

int main(int argc, char  *argv[]) {
    umask(0);                           /* So we get the permissions we want */
    if (mkfifo(FIFO_PATH, S_IRUSR | S_IWUSR) == -1 && errno != EEXIST)
        errExit("mkfifo %s", FIFO_PATH);
    if (initSemInUse(FIFO_PATH) == -1)
        errExit("initSemInUse error");
    switch (fork()) {
    case -1: errExit("fork error");
    case 0:
        printf("[%s %ld] Child started - doing some work\n", currTime("%T"), (long) getpid());
        sleep(2);               /* Simulate time spent doing some work */

        printf("[%s %ld] Child about to release semaphore\n", currTime("%T"), (long) getpid());
        if (releaseSem(FIFO_PATH) == -1)
            errExit("releaseSem error");
        /* Now child can do other things... */

        _exit(EXIT_SUCCESS);
    default:
        printf("[%s %ld] Parent about to wait for semaphore\n", currTime("%T"), (long) getpid());
        if (reserveSem(FIFO_PATH) == -1)
            errExit("reserveSem error");
        printf("[%s %ld] Parent about to continue\n", currTime("%T"), (long) getpid());
        if (unlink(FIFO_PATH) == -1)
            errExit("unlink error");
        exit(EXIT_SUCCESS);
    }
    return 0;
}
