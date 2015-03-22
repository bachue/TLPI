#define _GNU_SOURCE
#include <sys/resource.h>
#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    int prio;
    id_t who;

    if ((argc != 1 && argc < 4) || (argc >= 2 && strcmp(argv[1], "--help") == 0))
        usageErr("%s [-n prio cmd arg...]\n", argv[0]);

    if (argc >= 2 && strcmp(argv[1], "-n") == 0) {
        prio = getInt(argv[2], 0, "prio");
        switch (who = (id_t) fork()) {
        case -1:
            errExit("fork error");
        case 0:
            execvp(argv[3], &argv[3]);
            errExit("execv error");
        default:
            if (setpriority(PRIO_PROCESS, who, prio) == -1)
                errExit("setpriority error");
            if (waitpid((pid_t) who, NULL, 0) == -1)
                errExit("waitpid error");
        }
    } else {
        errno = 0;
        prio = getpriority(PRIO_PROCESS, getpid());
        if (prio == -1 && errno != 0)
            errExit("getpriority error");
        printf("%d\n", prio);
    }

    exit(EXIT_SUCCESS);
}
