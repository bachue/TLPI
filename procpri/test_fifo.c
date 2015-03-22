#define _GNU_SOURCE
#include <sched.h>
#include <time.h>
#include "tlpi_hdr.h"

static int clockTimeDiff(double targetTime) {
    clock_t clockTime;
    clockTime = clock();
    if (clockTime == -1)
        errExit("clockTime error");
    return ((double) clockTime / CLOCKS_PER_SEC) >= targetTime;
}

static void printInfo(char *prompt) {
    clock_t clockTime;
    clockTime = clock();
    if (clockTime == -1)
        errExit("clockTime error");
    printf("%s: pid = %ld, cputime = %f\n", prompt, (long) getpid(), (double) clockTime / CLOCKS_PER_SEC);
}

static void worker(char *prompt) {
    double prevClock = 0;
    printf("%s: Start working\n", prompt);
    for (;;) {
        if (clockTimeDiff(prevClock + 0.25)) {
            prevClock += 0.25;
            printInfo(prompt);
            if (prevClock - (int) prevClock == 0.0) {
                printf("%s: yield\n", prompt);
                if (sched_yield() == -1)
                    errExit("sched_yield error");
            }
            if (prevClock >= 10.00)
                break;
        }
    }
    printf("%s: worker died\n", prompt);
}

int main(int argc, char  *argv[]) {
    struct sched_param sp;
    sp.sched_priority = 99;
    switch (fork()) {
    case -1:
        errExit("fork error");
    case 0:
        if (sched_setscheduler(getpid(), SCHED_FIFO, &sp) == -1)
            errExit("sched_setscheduler error");
        worker("Child");
        _exit(EXIT_SUCCESS);
    default:
        if (sched_setscheduler(getpid(), SCHED_FIFO, &sp) == -1)
            errExit("sched_setscheduler error");
        worker("Parent");
        exit(EXIT_SUCCESS);
    }
}
