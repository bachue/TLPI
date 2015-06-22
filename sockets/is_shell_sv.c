#include "tlpi_hdr.h"
#include "inet_sockets.h"

#define BUF_SIZE 4096

void read_and_exec(int);

void read_and_exec(int cfd) {
    char command[BUF_SIZE];
    ssize_t numRead;

    while ((numRead = read(cfd, command, BUF_SIZE)) > 0) {
        switch (fork()) {
        case -1: errExit("fork error");
        case 0:
            if (close(STDIN_FILENO) == -1) errExit("close error");
            if (dup2(cfd, STDOUT_FILENO) == -1) errExit("dup2 error");
            if (dup2(cfd, STDERR_FILENO) == -1) errExit("dup2 error");
            execl("/bin/bash", "bash", "-c", command, NULL);
            errExit("exec error");
            break;
        }
    }

    if (numRead == -1) errExit("read command error");
}

int main(int argc, char const *argv[])
{
    int lfd, cfd;

    lfd = inetListen("5000", 10, NULL);
    if (lfd == -1)
        errExit("inetListen error");

    for (;;) {
        cfd = accept(lfd, NULL, NULL);  /* Wait for connection */
        if (cfd == -1)
            errExit("accept error");

        /* Handle each client request in a new child process */

        switch (fork()) {
        case -1: errExit("fork error");
        case 0:                         /* Child */
            close(lfd);                 /* Unneeded copy of listening socket */
            read_and_exec(cfd);
            _exit(EXIT_SUCCESS);

        default:                        /* Parent */
            close(cfd);                 /* Unneeded copy of connected socket */
            break;                      /* Loop to accept next connection */
        }
    }
    return 0;
}
