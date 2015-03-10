#include <signal.h>
#include <libgen.h>             /* For basename() declaration */
#include "tlpi_hdr.h"

#define CMD_SIZE 200

static void handler(int sig) {
    return;
}

int
main(int argc, char *argv[])
{
    char cmd[CMD_SIZE];
    pid_t childPid;
    sigset_t blockingMask, prevMask;
    struct sigaction sa;

    sigfillset(&blockingMask);

    if (sigprocmask(SIG_SETMASK, &blockingMask, &prevMask) == -1)
        errExit("sigprocmask");

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1)
        errExit("sigaction");
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    setbuf(stdout, NULL);       /* Disable buffering of stdout */

    printf("Parent PID=%ld\n", (long) getpid());

    switch (childPid = fork()) {
    case -1:
        errExit("fork");

    case 0:     /* Child: immediately exits to become zombie */
        printf("Child PID=%ld\n", (long) getpid());
        sigdelset(&blockingMask, SIGUSR1);
        sigsuspend(&blockingMask);
        printf("Child (PID=%ld) exiting\n", (long) getpid());
        if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
            errExit("Child: sigprocmask");
        _exit(EXIT_SUCCESS);
    default:    /* Parent */
        kill(childPid, SIGUSR1);               /* Give child a chance to start and exit */
        sigdelset(&blockingMask, SIGCHLD);
        sigsuspend(&blockingMask);
        snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
        system(cmd);            /* View zombie child */
        if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
            errExit("Parent: sigprocmask");

        /* Now send the "sure kill" signal to the zombie */

        if (kill(childPid, SIGKILL) == -1)
            errMsg("kill");
        sleep(3);               /* Give child a chance to react to signal */
        printf("After sending SIGKILL to zombie (PID=%ld):\n", (long) childPid);
        system(cmd);            /* View zombie child again */

        exit(EXIT_SUCCESS);
    }
}
