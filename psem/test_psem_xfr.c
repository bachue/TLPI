#include <semaphore.h>
#include <pthread.h>
#include "tlpi_hdr.h"

#define BUF_SIZE (1024 * 8)
#define READER ((void *) 1)
#define WRITER ((void *) 2)

static sem_t reader_sem, writer_sem;
static char buffer[BUF_SIZE];
static int bufsize;

static void *reader_worker(void*);
static void *writer_worker(void*);

void *reader_worker(void *arg) {
    int num;
    Boolean stop = FALSE;
    do {
        if (sem_wait(&reader_sem) == -1)
            errExit("sem_wait reader error");

        num = read(STDIN_FILENO, buffer, BUF_SIZE);
        if (num < 0) errExit("read error");
        else if (num == 0) stop = TRUE;
        bufsize = num;

        if (sem_post(&writer_sem) == -1)
            errExit("sem_post writer error");
    } while(stop == FALSE);

    return NULL;
}

void *writer_worker(void *arg) {
    Boolean stop = FALSE;
    do {
        if (sem_wait(&writer_sem) == -1)
            errExit("sem_wait writer error");

        if (bufsize == 0)
            stop = TRUE;
        else if (write(STDOUT_FILENO, buffer, bufsize) == -1)
            errExit("write error");

        if (sem_post(&reader_sem) == -1)
            errExit("sem_post reader error");
    } while(stop == FALSE);

    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t reader, writer;
    int s;
    if (sem_init(&reader_sem, 0, 1) == -1)
        errExit("sem_init reader error");

    if (sem_init(&writer_sem, 0, 0) == -1)
        errExit("sem_init writer error");

    s = pthread_create(&reader, NULL, reader_worker, READER);
    if (s != 0) errExitEN(s, "pthread_create reader error");

    s = pthread_create(&writer, NULL, writer_worker, WRITER);
    if (s != 0) errExitEN(s, "pthread_create writer error");

    s = pthread_join(reader, NULL);
    if (s != 0) errExitEN(s, "pthread_join reader error");

    s = pthread_join(writer, NULL);
    if (s != 0) errExitEN(s, "pthread_join writer error");

    exit(EXIT_SUCCESS);
}
