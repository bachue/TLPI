#define _GNU_SOURCE
#include <sched.h>
#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    uid_t ruid, euid, suid;
    int policy;
    pid_t pid;
    struct sched_param sp;
    if (argc < 4 || (argc >= 2 && strcmp("--help", argv[1]) == 0))
        usageErr("%s policy priority command arg ...\n", argv[0]);

    if (strcmp(argv[1], "r") == 0)
        policy = SCHED_RR;
    else if (strcmp(argv[1], "f") == 0)
        policy = SCHED_FIFO;
    else
        usageErr("%s r|f priority command arg ...", argv[0]);

    sp.sched_priority = getInt(argv[2], 0, "priority");

    if (getresuid(&ruid, &euid, &suid) == -1)
        errExit("getresuid error");

    switch (pid = fork()) {
    case -1:
        errExit("fork error");
    case 0:
        if (euid == 0 && (ruid != 0 || suid != 0)) {
            if (setresuid(ruid, ruid, ruid) == -1)
                errExit("setresuid error");
        }
        execvp(argv[3], &argv[3]);
        errExit("execvp error");
    default:
        if (sched_setscheduler(pid, policy, &sp) == -1)
            errExit("sched_setscheduler error");
        if (waitpid(pid, 0, NULL) == -1)
            errExit("sched_setscheduler error");
    }

    exit(EXIT_SUCCESS);
}
