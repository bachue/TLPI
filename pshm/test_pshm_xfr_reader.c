#include "test_pshm_xfr.h"

int main(int argc, char const *argv[])
{
    int fd;
    struct shmseg *addr;
    Boolean stop;
    sem_t *sem_reader, *sem_writer;

    sem_reader = sem_open(SEM_READER_PATH, 0);
    if (sem_reader == SEM_FAILED)
        errExit("sem_open reader error");

    sem_writer = sem_open(SEM_WRITER_PATH, 0);
    if (sem_writer == SEM_FAILED)
        errExit("sem_open reader error");

    fd = shm_open(SHM_PATH, O_RDWR, 0);
    if (fd == -1)
        errExit("shm_open error");

    addr = mmap(NULL, sizeof(struct shmseg), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        errExit("mmap error");
    if (close(fd) == -1)
        errExit("close error");

    stop = FALSE;
    do {
        if (sem_wait(sem_reader) == -1)
            errExit("sem_wait reader error");

        if (addr->cnt == 0)
            stop = TRUE;
        else if (write(STDOUT_FILENO, addr->buf, addr->cnt) == -1)
            errExit("write error");

        if (sem_post(sem_writer) == -1)
            errExit("sem_wait writer error");
    } while(stop == FALSE);

    if (sem_close(sem_reader) == -1)
        errExit("sem_close reader error");
    if (sem_close(sem_writer) == -1)
        errExit("sem_close writer error");

    return 0;
}
