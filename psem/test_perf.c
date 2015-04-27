#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

static void test_svsem(int);
static void test_psem(int);

void test_svsem(int loop) {
    int semid, cnt;
    struct sembuf ops;

    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (semid == -1)
        errExit("semget error");

    for (cnt = 0; cnt < loop; cnt++) {
        ops.sem_num = 0;
        ops.sem_op = 1;
        ops.sem_flg = 0;
        if (semop(semid, &ops, 1) == -1)
            errExit("semop error");

        ops.sem_num = 0;
        ops.sem_op = -1;
        ops.sem_flg = 0;
        if (semop(semid, &ops, 1) == -1)
            errExit("semop error");
    }

    if (semctl(semid, 0, IPC_RMID) == -1)
        errExit("remove sem error");
}

void test_psem(int loop) {
    int cnt;
    sem_t sem;
    if (sem_init(&sem, 0, 1) == -1)
        errExit("sem_init error");

    for (cnt = 0; cnt < loop; cnt++) {
        if (sem_post(&sem) == -1)
            errExit("sem_post error");
        if (sem_wait(&sem) == -1)
            errExit("sem_wait error");
    }
}

int main(int argc, char const *argv[])
{
    int loop;
    if (argc != 3)
        usageErr("%s svsem|psem num-loop\n", argv[0]);
    loop = getInt(argv[2], GN_GT_0, "num-loops");
    if (strcmp(argv[1], "svsem") == 0) {
        test_svsem(loop);
    } else if (strcmp(argv[1], "psem") == 0) {
        test_psem(loop);
    } else
        usageErr("%s svsem|psem num-loop", argv[0]);
    return 0;
}
