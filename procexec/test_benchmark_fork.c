#include "tlpi_hdr.h"
#include <sys/time.h>
#include <sys/wait.h>

int main(int argc, char  *argv[]) {
    int num, i;
    pid_t pid;
    struct timeval start, end;
    time_t sec_diff;
    suseconds_t usec_diff;
    if (argc < 3)
        usageErr("%s fork|vfork times\n", argv[0]);
    num = getInt(argv[2], GN_GT_0, "times");

    if (gettimeofday(&start, NULL) == -1)
        errExit("gettimeofday error");

    for (i = 0; i < num; ++i) {
        if (strcmp(argv[1], "fork") == 0) {
            switch (pid = fork()) {
            case -1:
                errExit("fork error");
            case 0:
                _exit(EXIT_SUCCESS);
            default:
                waitpid(pid, NULL, 0);
                break;
            }
        } else if (strcmp(argv[1], "vfork") == 0) {
            switch (pid = vfork()) {
            case -1:
                errExit("vfork error");
            case 0:
                _exit(EXIT_SUCCESS);
            default:
                waitpid(pid, NULL, 0);
                break;
            }
        } else
            usageErr("%s fork|vfork", argv[0]);
    }

    if (gettimeofday(&end, NULL) == -1)
        errExit("gettimeofday error");

    sec_diff = end.tv_sec - start.tv_sec;
    usec_diff = end.tv_usec - start.tv_usec;

    if (usec_diff < 0) {
        usec_diff += 1000000;
        sec_diff -= 1;
    }
    printf("Time: %ld.%06ld\n", (long) sec_diff, (long) usec_diff);

    return 0;
}
