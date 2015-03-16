#include <pthread.h>
#include <sys/signal.h>
#include <sys/types.h>
#include "tlpi_hdr.h"

static void *thread_func(void *arg) {
    int s, sig;
    sigset_t set;
    Boolean blocked;
    char buf[2049];

    sig = (int) arg;

    sigemptyset(&set);
    sigaddset(&set, sig);

    s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (s != 0)
        errExitEN(s, "pthread_sigmask");

    for (;;) {
        sleep(1);
        sigpending(&set);
        snprintf(buf, 2049, "%ld: Signal pending: ", (long) pthread_self());
        blocked = FALSE;
        for (sig = 1; sig < NSIG; sig++) {
            if (sigismember(&set, sig)) {
                if (blocked == TRUE)
                    strncat(buf, ", ", 2048);
                else
                    blocked = TRUE;
                strncat(buf, strsignal(sig), 2048);
            }
        }
        if (blocked == FALSE)
            strncat(buf, "<empty signal set>", 2048);
        buf[2048] = '\0';
        printf("%s\n", buf);
    }
}

int main(int argc, char  *argv[]) {
    pthread_t t1, t2;
    sigset_t set;
    int s;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (s != 0)
        errExitEN(s, "pthread_sigmask");

    s = pthread_create(&t1, NULL, thread_func, (void *) SIGUSR1);
    if (s != 0)
        errExitEN(s, "pthread_sigmask");

    s = pthread_create(&t2, NULL, thread_func, (void *) SIGUSR2);
    if (s != 0)
        errExitEN(s, "pthread_sigmask");

    sleep(2);

    s = pthread_kill(t1, SIGINT);
    printf("Sent SIGINT to Thread 1\n");
    sleep(1);

    s = pthread_kill(t2, SIGINT);
    printf("Sent SIGINT to Thread 2\n");
    sleep(1);

    s = pthread_kill(t1, SIGUSR1);
    printf("Sent SIGUSR1 to Thread 1\n");
    sleep(1);

    s = pthread_kill(t2, SIGUSR2);
    printf("Sent SIGUSR2 to Thread 2\n");
    sleep(1);

    return 0;
}
