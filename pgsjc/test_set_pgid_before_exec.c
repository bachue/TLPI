#include <signal.h>
#include "tlpi_hdr.h"

void printinfo(char*);

void printinfo(char *prompt) {
    printf("%s: PID=%ld, PPID=%ld, PGID=%ld, SID=%ld\n", prompt,
            (long) getpid(), (long) getppid(),
            (long) getpgrp(), (long) getsid(0));
}

int main(int argc, char  *argv[]) {
    pid_t pid;

    switch (pid = fork()) {
    case -1:
        errExit("fork error");
    case 0:
        printinfo("Child");
        sleep(2);
        printinfo("Child");
        printf("Child process finished\n");
        execlp("sleep", "sleep", "5", NULL);
        errExit("execlp error");
    default:
        printinfo("Parent");
        sleep(1);
        if (setpgid(pid, pid) == -1)
            errExit("setpgid error");
        sleep(2);
        if (setpgid(pid, pid) == -1)
            errExit("setpgid error");
        break;
    }
    return 0;
}
