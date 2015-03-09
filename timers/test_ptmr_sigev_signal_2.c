#define _POSIX_C_SOURCE 199309
#include <signal.h>
#include <time.h>
#include "curr_time.h"                  /* Declares currTime() */
#include "itimerspec_from_str.h"        /* Declares itimerspecFromStr() */
#include "tlpi_hdr.h"

#define TIMER_SIG SIGRTMAX              /* Our timer notification signal */

static void handle_siginfo(siginfo_t *si)
{
    timer_t *tidptr;

    tidptr = si->si_value.sival_ptr;

    printf("[%s] Got signal %d\n", currTime("%T"), si->si_signo);
    printf("    *sival_ptr         = %ld\n", (long) *tidptr);
    printf("    timer_getoverrun() = %d\n", timer_getoverrun(*tidptr));
}

int
main(int argc, char *argv[])
{
    struct itimerspec ts;
    struct sigevent   sev;
    sigset_t prevMask, blockingMask;
    siginfo_t siginfo;
    timer_t *tidlist;
    int j;

    if (argc < 2)
        usageErr("%s secs[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);

    tidlist = calloc(argc - 1, sizeof(timer_t));
    if (tidlist == NULL)
        errExit("malloc");

    sigemptyset(&blockingMask);
    sigaddset(&blockingMask, TIMER_SIG);

    if (sigprocmask(SIG_SETMASK, &blockingMask, &prevMask) == -1)
      errExit("sigprocmask");

    /* Create and start one timer for each command-line argument */

    sev.sigev_notify = SIGEV_SIGNAL;    /* Notify via signal */
    sev.sigev_signo = TIMER_SIG;        /* Notify using this signal */

    for (j = 0; j < argc - 1; j++) {
        itimerspecFromStr(argv[j + 1], &ts);

        sev.sigev_value.sival_ptr = &tidlist[j];
                /* Allows handler to get ID of this timer */

        if (timer_create(CLOCK_REALTIME, &sev, &tidlist[j]) == -1)
            errExit("timer_create");
        printf("Timer ID: %ld (%s)\n", (long) tidlist[j], argv[j + 1]);

        if (timer_settime(tidlist[j], 0, &ts, NULL) == -1)
            errExit("timer_settime");
    }

    for (;;) {                           /* Wait for incoming timer signals */
      if (sigwaitinfo(&blockingMask, &siginfo) == -1)
        errExit("sigwaitinfo");
      handle_siginfo(&siginfo);
    }

    if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
      errExit("sigprocmask");
}
