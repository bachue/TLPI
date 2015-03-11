#include "tlpi_hdr.h"
#include <signal.h>
#include <sys/wait.h>

static void handler(int signum) {
    char buf[64];
    snprintf(buf, 63, "Caught signal %d (%s)\n", signum, strsignal(signum));
    buf[63] = '\0';
    write(STDOUT_FILENO, buf, strlen(buf));
}

int main(int argc, char  *argv[]) {
    struct sigaction sa;
    sigset_t block_mask, orig_mask;
    pid_t pid;

    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &block_mask, &orig_mask) == -1)
        errExit("sigprocmask error");

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction error");

    switch (pid = fork()) {
    case -1:
        errExit("fork error");
    case 0:
        printf("Child: born & dead\n");
        _exit(EXIT_SUCCESS);
    default:
        if (waitpid(pid, (int *) NULL, 0) == -1)
            errExit("waitpid error");
        printf("Parent: waited\n");
    }

    if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) == -1)
        errExit("sigprocmask error");

    exit(EXIT_SUCCESS);
}
