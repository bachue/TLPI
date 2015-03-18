#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    switch (fork()) {
    case -1:
        errExit("fork error");
    case 0:
        sleep(1);
        printf("PID=%ld; PPID=%ld; PGID=%ld; SID=%ld\n", (long) getpid(),
                (long) getppid(), (long) getpgrp(), (long) getsid(0));
        exit(EXIT_SUCCESS);
    default:
        execlp("echo", "echo", "Parent process died", NULL);
        errExit("execlp error");
    }
    return 0;
}
