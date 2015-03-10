/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 26-3 */

/* child_status.c

   Demonstrate the use of wait() and the W* macros for analyzing the child
   status returned by wait()

   Usage: child_status [exit-status]

   If "exit-status" is supplied, then the child immediately exits with this
   status. If no command-line argument is supplied then the child loops waiting
   for signals that either cause it to stop or to terminate - both conditions
   can be detected and differentiated by the parent. The parent process
   repeatedly waits on the child until it detects that the child either exited
   normally or was killed by a signal.
*/
#include <sys/wait.h>
#include "print_wait_status.h"          /* Declares printWaitStatus() */
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    siginfo_t siginfo;
    int result;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [exit-status]\n", argv[0]);

    switch (fork()) {
    case -1: errExit("fork");

    case 0:             /* Child: either exits immediately with given
                           status or loops waiting for signals */
        printf("Child started with PID = %ld\n", (long) getpid());
        if (argc > 1)                   /* Status supplied on command line? */
            exit(getInt(argv[1], 0, "exit-status"));
        else                            /* Otherwise, wait for signals */
            for (;;)
                pause();
        exit(EXIT_FAILURE);             /* Not reached, but good practice */

    default:            /* Parent: repeatedly wait on child until it
                           either exits or is terminated by a signal */
        for (;;) {
            result = waitid(P_ALL, 0, &siginfo, WEXITED | WUNTRACED
#ifdef WCONTINUED       /* Not present on older versions of Linux */
                                                | WCONTINUED
#endif
                    );
            if (result == -1)
                errExit("waitid");

            /* Print status in hex, and as separate decimal bytes */

            printf("waitpid() returned: PID=%ld; status=%u, signal=%d (%s)\n",
                    (long) siginfo.si_pid,
                    (unsigned int) siginfo.si_status, siginfo.si_signo, strsignal(siginfo.si_signo));

            switch (siginfo.si_code) {
            case CLD_EXITED:
                printf("child exited, status=%d\n", siginfo.si_status);
                break;
            case CLD_DUMPED:
                printf("child killed by signal %d (%s)\n", siginfo.si_status, strsignal(siginfo.si_status));
                printf(" (core dumped)\n");
                break;
            case CLD_STOPPED:
                printf("child stopped by signal %d (%s)\n", siginfo.si_status, strsignal(siginfo.si_status));
                break;
            case CLD_CONTINUED:
                printf("child continued\n");
                break;
            case CLD_KILLED:
                printf("child killed by signal %d (%s)\n", siginfo.si_status, strsignal(siginfo.si_status));
                break;
            default:
                printf("what happened to this child?\n");
            }

            if (siginfo.si_code == CLD_EXITED || siginfo.si_code == CLD_KILLED)
                exit(EXIT_SUCCESS);
        }
    }
}
