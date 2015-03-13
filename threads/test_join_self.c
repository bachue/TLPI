#include <pthread.h>
#include "tlpi_hdr.h"

static void* threadFunc(void *arg) {
    printf("In new thread\n");
    errExitEN(pthread_join(pthread_self(), NULL), "pthread_join in new thread");
    printf("After join\n");
    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t t1;
    int s;

    s = pthread_create(&t1, NULL, threadFunc, NULL);
    if (s != 0)
        errExitEN(s, "pthread_create");

    // errExitEN(pthread_join(pthread_self(), (void **) &s), "pthread_join in main process");
    sleep(2);

    exit(EXIT_SUCCESS);
}
