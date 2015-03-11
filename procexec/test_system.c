#include "tlpi_hdr.h"
#include <signal.h>

static void handler(int signum) {
    char buf[64];
    snprintf(buf, 63, "Caught signal %d (%s)\n", signum, strsignal(signum));
    buf[63] = '\0';
    write(STDOUT_FILENO, buf, strlen(buf));
}

int main(int argc, char  *argv[]) {
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction error");

    printf("Before system()\n");
    if (system("sleep 1") == -1)
        errExit("system error");
    printf("After system()\n");

    sleep(1);

    return 0;
}
