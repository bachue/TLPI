#include <limits.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

static void print_getppid(const char *prompt) {
    printf("%s ppid:  %d\n", prompt, getppid());
}

int main(int argc, char  *argv[]) {
    pid_t pid;
    switch (fork()) {
    case -1:
        errExit("Parent: fork error");
    case 0:
        printf("Child: %d\n", getpid());
        switch (fork()) {
        case -1:
            errExit("Child: fork error");
        case 0:
            printf("GrandChild: %d\n", getpid());
            print_getppid("GrandChild");
            printf("\n");
            sleep(1);
            print_getppid("GrandChild");
            printf("\n");
            sleep(3);
            print_getppid("GrandChild");
            printf("GrandChild: dead\n");
            printf("\n");
            _exit(EXIT_SUCCESS);
        default:
            sleep(1);
            printf("Child: dead\n");
            _exit(EXIT_SUCCESS);
        }
    default:
        printf("Parent: finished forking\n");
        sleep(2);
        pid = wait(NULL);
        if (pid == -1)
            perror("wait error");
        else
            printf("Parent: %d sends SIGCHLD\n", pid);
        sleep(4);
        exit(EXIT_SUCCESS);
    }
}
