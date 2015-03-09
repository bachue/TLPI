#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void sigHandler(int signum) {
    char buf[64];
    size_t size;
    snprintf(buf, 63, "Caught signal %d (%s)\n", signum, strsignal(signum));
    buf[63] = '\0';
    size = strlen(buf);
    if (write(STDOUT_FILENO, buf, size) == -1)
        write(STDERR_FILENO, buf, size);
}

static unsigned my_alarm(unsigned seconds) {
    struct itimerval newval, oldval;
    newval.it_value.tv_sec = (time_t) seconds;
    newval.it_value.tv_usec = 0;
    newval.it_interval.tv_sec = 0;
    newval.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &newval, &oldval) == -1)
        errExit("setitimer error");
    return (unsigned) oldval.it_value.tv_sec;
}

int main(int argc, char  *argv[]) {
    struct sigaction sa;
    sigset_t blockingMask, prevMask, emptyMask;

    printf("%s: PID: %d\n", argv[0], getpid());

    sigemptyset(&emptyMask);
    sigemptyset(&blockingMask);
    sigaddset(&blockingMask, SIGALRM);
    if (sigprocmask(SIG_SETMASK, &blockingMask, &prevMask) == -1)
        errExit("sigprocmask error");

    sa.sa_handler = sigHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        errExit("sigaction error");
    my_alarm(1);
    for (;;) {
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend issue");
    }
    if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
        errExit("sigprocmask error");
    exit(EXIT_SUCCESS);
}
