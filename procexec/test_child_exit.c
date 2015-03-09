#include "tlpi_hdr.h"
#include <sys/wait.h>

int main(int argc, char  *argv[]) {
    pid_t pid;
    int status;
    switch (pid = fork()) {
        case -1:
            errExit("fork error");
        case 0:
            exit(-1);
        default:
            waitpid(pid, &status, 0);
            printf("exit: %d\n", WEXITSTATUS(status));
            exit(EXIT_SUCCESS);
    }
}
