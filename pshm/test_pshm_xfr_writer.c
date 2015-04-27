#include "test_pshm_xfr.h"

int main(int argc, char const *argv[])
{
    int fd, readNum;
    struct shmseg *addr;
    Boolean stop;
    sem_t *sem_reader, *sem_writer;

    sem_reader = sem_open(SEM_READER_PATH, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (sem_reader == SEM_FAILED)
        errExit("sem_open reader error");

    sem_writer = sem_open(SEM_WRITER_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (sem_writer == SEM_FAILED)
        errExit("sem_open reader error");

    fd = shm_open(SHM_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("shm_open error");
    if (ftruncate(fd, sizeof(struct shmseg)) == -1)
        errExit("ftruncate error");

    addr = mmap(NULL, sizeof(struct shmseg), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        errExit("mmap error");
    if (close(fd) == -1)
        errExit("close error");

    stop = FALSE;
    do {
        if (sem_wait(sem_writer) == -1)
            errExit("sem_wait writer error");

        readNum = read(STDIN_FILENO, addr->buf, BUF_SIZE);
        if (readNum == -1)
            errExit("read error");
        addr->cnt = readNum;
        if (readNum == 0)
            stop = TRUE;

        if (sem_post(sem_reader) == -1)
            errExit("sem_wait reader error");
    } while(stop == FALSE);

    if (sem_close(sem_reader) == -1)
        errExit("sem_close reader error");
    if (sem_close(sem_writer) == -1)
        errExit("sem_close writer error");
    if (sem_unlink(SEM_READER_PATH) == -1)
        errExit("sem_unlink reader error");
    if (sem_unlink(SEM_WRITER_PATH) == -1)
        errExit("sem_unlink writer error");
    if (shm_unlink(SHM_PATH) == -1)
        errExit("shm_unlink error");

    return 0;
}
