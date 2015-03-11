#include "tlpi_hdr.h"
#include <fcntl.h>
#include <signal.h>

static void handler(int signum) {
    write(STDOUT_FILENO, "Child died, test finished\n", strlen("Child died, test finished\n"));
    exit(EXIT_SUCCESS);
}

int main(int argc, char  *argv[]) {
    pid_t pid;
    sigset_t sigset;
    struct sigaction sa;

    switch (pid = fork()) {
    case -1:
        errExit("fork error");
    case 0:
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGQUIT);
        if (sigprocmask(SIG_BLOCK, &sigset, NULL) == -1)
            errExit("sigprocmask error");
        execlp("sleep", "sleep", "5", NULL);
        errExit("execlp error");
    default:
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = handler;
        sa.sa_flags = 0;
        if (sigaction(SIGCHLD, &sa, NULL) == -1)
            errExit("sigaction error");
        for (;;) {
            sleep(1);
            if (kill(pid, SIGQUIT) == -1)
                errExit("kill error");
            else
                printf("Trying to kill %ld\n", (long) pid);
        }
        break;
    }
    return 0;
}
