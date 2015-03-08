#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include "signal_functions.h"           /* Declaration of printSigset() */
#include "tlpi_hdr.h"

#define BUF_SIZE 63

static void sig_handler(int sig) {
    char buf[BUF_SIZE + 1];
    snprintf(buf, BUF_SIZE, "Child: Caught Signal %d (%s)\n", sig, strsignal(sig));
    buf[BUF_SIZE] = '\0';
    if (write(STDOUT_FILENO, buf, strlen(buf)) <= 0) {
        write(STDERR_FILENO, "Child: Failed to write to STDOUT\n", strlen("Failed to write to STDOUT\n"));
        _exit(-2);
    }
}

static int invalid_signum(int signum) {
    return signum <= 0 || signum == SIGKILL || signum == SIGSTOP || signum > SIGRTMAX || (signum > SIGSYS && signum < SIGRTMIN);
}

static void setup_signal_handlers_for_test() {
    int i;
    struct sigaction sa;
    sigset_t prev_mask, block_mask, pending_mask;
    printf("Child: PID is %ld\n", (long) getpid());

    sigfillset(&block_mask);
    printf("Child: Start blocking all signals\n");
    if (sigprocmask(SIG_SETMASK, &block_mask, &prev_mask) == -1)
        errExit("Child: sigprocmask error");

    sa.sa_handler = sig_handler;
    sa.sa_flags = 0;
    for (i = 1; i <= SIGRTMAX; ++i) {
        if (invalid_signum(i))
            continue;
        if (sigaction(i, &sa, NULL) == -1)
            errExit("Child: sigaction error");
    }

    printf("Child: All signals setup, ask parent to test it ...\n");
    if (kill(getppid(), SIGUSR1) == -1)
        errExit("Child: kill error");

    printf("Child: Hope parent receive the signal, start sleeping\n");
    sleep(10);

    printf("Child: Start unblocking all signals\n");
    if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1)
        errExit("Child: sigprocmask error");

    if (sigpending(&pending_mask) == -1)
        errExit("Child: sigpending error");
    printf("Child: pending signals are: \n");
    printSigset(stdout, "\t\t", &pending_mask);
}

static void wait_for_testing_child(pid_t pid) {
    sigset_t wait_mask, prev_mask;
    siginfo_t siginfo;
    int i, sig;

    srand(time(NULL));

    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, SIGUSR1);

    if (sigprocmask(SIG_SETMASK, &wait_mask, &prev_mask) == -1)
        errExit("Parent: sigprocmask error");

    printf("Parent: Ready for waiting the signal\n");
    if (sigwaitinfo(&wait_mask, &siginfo) == -1)
        errExit("Parent: sigwaitinfo error");
    printf("Parent: Signal Caught, start testing, will send 1000 random signals to the child\n");

    for (i = 0; i < 1000; ++i) {
        do {
            sig = rand() % SIGRTMAX + 1;
        } while (invalid_signum(sig));
        printf("Parent: send %d (%s)\n", sig, strsignal(sig));
        if (kill(pid, sig) == -1)
            errExit("Parent: sending signal error");
    }
    if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1)
        errExit("Parent: sigprocmask error");
    printf("Parent: Test finished, waiting for child awake\n");
    waitpid(pid, NULL, 0);
}

int main(int argc, char  *argv[]) {
    pid_t pid;
    switch (pid = fork()) {
        case -1:
            errExit("fork error");
            break;
        case 0: /* Child */
            setup_signal_handlers_for_test();
            break;
        default: /* Parent */
            wait_for_testing_child(pid);
            break;
    }
    return 0;
}
