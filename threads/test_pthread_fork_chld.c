#include <pthread.h>
#include <sys/signal.h>
#include <sys/types.h>
#include "tlpi_hdr.h"

static void* thread_func(void *arg) {
    sigset_t set;
    int s, sig;
    pthread_t self = pthread_self();

    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (s != 0)
        errExitEN(s, "pthread_sigmask");

    if ((int) (arg) == 1) {
        printf("%ld: ready to fork\n", (long) self);
        switch (fork()) {
        case -1:
            errExit("fork error");
        case 0:
            sleep(1);
            _exit(EXIT_SUCCESS);
        default:
            ;
        }
    }

    printf("%ld: Waiting for SIGCHLD signal\n", (long) self);
    while (sigwait(&set, &sig) == 0)
        printf("%ld: Caught signal %s\n", (long) self, strsignal(sig));

    return NULL;
}

int main(int argc, char  *argv[]) {
    pthread_t t1, t2, t3, t4, t5;
    int s;

    s = pthread_create(&t1, NULL, thread_func, (void *) 1);
    if (s != 0)
        errExitEN(s, "pthread_create - 1");

    s = pthread_create(&t2, NULL, thread_func, (void *) 0);
    if (s != 0)
        errExitEN(s, "pthread_create - 2");

    s = pthread_create(&t3, NULL, thread_func, (void *) 1);
    if (s != 0)
        errExitEN(s, "pthread_create - 3");

    s = pthread_create(&t4, NULL, thread_func, (void *) 0);
    if (s != 0)
        errExitEN(s, "pthread_create - 4");

    s = pthread_create(&t5, NULL, thread_func, (void *) 0);
    if (s != 0)
        errExitEN(s, "pthread_create - 5");

    sleep(3);

    return 0;
}
