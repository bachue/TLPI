#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "curr_time.h"                  /* Declaration of currTime() */
#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    int semid;
    struct sembuf sops;

    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (semid == -1)
        errExit("semget error");
    switch (fork()) {
    case -1: errExit("fork error");
    case 0:
        printf("[%s %ld] Child started - doing some work\n", currTime("%T"), (long) getpid());
        sleep(2);               /* Simulate time spent doing some work */

        printf("[%s %ld] Child about to release semaphore\n", currTime("%T"), (long) getpid());
        sops.sem_num = 0;
        sops.sem_op = 1;
        sops.sem_flg = 0;

        if (semop(semid, &sops, 1) == -1)
            errExit("Child: release semaphore error");
        /* Now child can do other things... */

        _exit(EXIT_SUCCESS);
    default:
        printf("[%s %ld] Parent about to wait for semaphore\n", currTime("%T"), (long) getpid());
        sops.sem_num = 0;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if (semop(semid, &sops, 1) == -1)
            errExit("Parent: wait for semaphore error");
        printf("[%s %ld] Parent about to continue\n", currTime("%T"), (long) getpid());
        if (semctl(semid, 0, IPC_RMID) == -1)
            errExit("Parent: remove semaphore error");
        exit(EXIT_SUCCESS);
    }
}
