#include <pthread.h>
#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    pthread_mutex_t mtx;
    pthread_mutexattr_t mtxAttr;
    int s;

    s = pthread_mutexattr_init(&mtxAttr);
    if (s != 0)
        errExitEN(s, "pthread_mutexattr_init");

    s = pthread_mutexattr_settype(&mtxAttr, PTHREAD_MUTEX_RECURSIVE);
    if (s != 0)
        errExitEN(s, "pthread_mutexattr_settype");

    s = pthread_mutex_init(&mtx, &mtxAttr);
    if (s != 0)
        errExitEN(s, "pthread_mutexattr_init");

    s = pthread_mutexattr_destroy(&mtxAttr);
    if (s != 0)
        errExitEN(s, "pthread_mutexattr_destroy");

    s = pthread_mutex_lock(&mtx);
    if (s != 0)
        errExitEN(s, "pthread_mutex_lock 1");

    printf("Locked - 1\n");

    s = pthread_mutex_lock(&mtx);
    if (s != 0)
        errExitEN(s, "pthread_mutex_lock 2");

    printf("Locked - 2\n");

    s = pthread_mutex_unlock(&mtx);
    if (s != 0)
        errExitEN(s, "pthread_mutex_unlock 2");

    printf("Unlocked - 2\n");

    s = pthread_mutex_unlock(&mtx);
    if (s != 0)
        errExitEN(s, "pthread_mutex_unlock 1");

    printf("Unlocked - 1\n");

    return 0;
}
