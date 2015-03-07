#include <string.h>
#include <signal.h>
#include "signal_functions.h"           /* Declaration of printSigset() */
#include "tlpi_hdr.h"

#define BUF_SIZE 63

static void sigcont_handler(int sig) {
    char buf[BUF_SIZE + 1];
    snprintf(buf, BUF_SIZE, "Caught Signal %d (%s)\n", sig, strsignal(sig));
    buf[BUF_SIZE] = '\0';
    if (write(STDOUT_FILENO, buf, strlen(buf)) <= 0) {
        write(STDERR_FILENO, "Failed to write to STDOUT\n", strlen("Failed to write to STDOUT\n"));
        _exit(-2);
    }
}

int main(int argc, char  *argv[]) {
    struct sigaction sa;
    sigset_t prevMask, blockMask, pendingMask;

    printf("%s: PID is %ld\n", argv[0], (long) getpid());

    sa.sa_handler = sigcont_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCONT, &sa, NULL) == -1)
        errExit("sigaction error");
    if (sigaction(SIGUSR1, &sa, NULL) == -1)
        errExit("sigaction error");

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCONT);
    sigaddset(&blockMask, SIGUSR1);

    printf("Start blocking SIGCONT & SIGUSR1\n");
    if (sigprocmask(SIG_SETMASK, &blockMask, &prevMask) == -1)
        errExit("sigprocmask error");

    printf("Start sleeping ...\n");
    sleep(10);
    printf("Stop sleeping ...\n");

    if (sigpending(&pendingMask) == -1)
        errExit("sigpending");
    printf("%s: pending signals are: \n", argv[0]);
    printSigset(stdout, "\t\t", &pendingMask);

    printf("Stop blocking SIGCONT & SIGUSR1\n");
    if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
        errExit("sigprocmask error");

    if (sigpending(&pendingMask) == -1)
        errExit("sigpending");
    printf("%s: pending signals are: \n", argv[0]);
    printSigset(stdout, "\t\t", &pendingMask);
    exit(EXIT_SUCCESS);
}
