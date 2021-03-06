/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 64-3 */

/* script.c

   A simple version of script(1).
*/
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>
#include "pty_fork.h"           /* Declaration of ptyFork() */
#include "tty_functions.h"      /* Declaration of ttySetRaw() */
#include "tlpi_hdr.h"
#include <time.h>
#include <signal.h>

#define BUF_SIZE 256
#define MAX_SNAME 1000

static struct termios ttyOrig;
static int scriptFd, masterFd;
static char *scriptPath;

static void gettime(struct tm *t) {
    time_t rawtime;
    time(&rawtime);
    localtime_r(&rawtime, t);
}

static void ttyReset(void) {
    char buf[BUF_SIZE];
    struct tm endtime;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &ttyOrig) == -1)
        errExit("tcsetattr");
    printf("Script done, file is %s\n", scriptPath);
    gettime(&endtime);
    strftime(buf, BUF_SIZE, "%a %b %d %H:%M:%S %Y", &endtime);
    dprintf(scriptFd, "Script done on %s\n", buf);
    if (close(scriptFd) == -1)
        errExit("close");
}

static void sigwinchHandler(int sig) {
    struct winsize ws;
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
        errExit("ioctl-TIOCGWINSZ");
    if (ioctl(masterFd, TIOCSWINSZ, &ws) < 0)
        errExit("ioctl-TIOCSWINSZ");
}

int
main(int argc, char *argv[])
{
    char slaveName[MAX_SNAME];
    char *shell, buf[BUF_SIZE];
    struct winsize ws;
    ssize_t numRead;
    pid_t childPid;
    struct tm begtime;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigwinchHandler;
    if (sigaction(SIGWINCH, &sa, NULL) == -1)
        errExit("sigaction");

    /* Retrieve the attributes of terminal on which we are started */

    if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
        errExit("tcgetattr");
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
        errExit("ioctl-TIOCGWINSZ");

    /* Create a child process, with parent and child connected via a
       pty pair. The child is connected to the pty slave and its terminal
       attributes are set to be the same as those retrieved above. */

    childPid = ptyFork(&masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws);
    if (childPid == -1)
        errExit("ptyFork");

    if (childPid == 0) {        /* Child: execute a shell on pty slave */

        /* If the SHELL variable is set, use its value to determine
           the shell execed in child. Otherwise use /bin/sh. */

        shell = getenv("SHELL");
        if (shell == NULL || *shell == '\0')
            shell = "/bin/sh";

        execlp(shell, shell, (char *) NULL);
        errExit("execlp");      /* If we get here, something went wrong */
    }

    /* Parent: relay data between terminal and pty master */

    scriptPath = (argc > 1) ? argv[1] : "typescript";
    scriptFd = open(scriptPath, O_WRONLY | O_CREAT | O_TRUNC,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                                S_IROTH | S_IWOTH);
    if (scriptFd == -1)
        errExit("open typescript");

    /* Place terminal in raw mode so that we can pass all terminal
     input to the pseudoterminal master untouched */

    printf("Script started, file is %s\n", scriptPath);

    gettime(&begtime);
    strftime(buf, BUF_SIZE, "%a %b %d %H:%M:%S %Y", &begtime);
    dprintf(scriptFd, "Script started on %s\n", buf);
    ttySetRaw(STDIN_FILENO, &ttyOrig);

    if (atexit(ttyReset) != 0)
        errExit("atexit");

    switch (fork()) {
    case -1: errExit("fork");
    case 0:
        if (close(scriptFd) == -1)
            errExit("close");
        for (;;) {
            numRead = read(STDIN_FILENO, buf, BUF_SIZE);
            if (numRead <= 0)
                _exit(EXIT_SUCCESS);

            if (write(masterFd, buf, numRead) != numRead)
                fatal("partial/failed write (masterFd)");
        }
        break;
    default:
        for (;;) {
            numRead = read(masterFd, buf, BUF_SIZE);
            if (numRead <= 0)
                exit(EXIT_SUCCESS);

            if (write(STDOUT_FILENO, buf, numRead) != numRead)
                fatal("partial/failed write (STDOUT_FILENO)");
            if (write(scriptFd, buf, numRead) != numRead)
                fatal("partial/failed write (scriptFd)");
        }
        break;
    }
}
