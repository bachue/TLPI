#include "tlpi_hdr.h"
#include <sys/time.h>
#include <sys/wait.h>
#include <limits.h>

#define TOTAL ( 8l * 1024 * 1024 * 1024 ) // 8 GB
#define BUF_SIZE PIPE_BUF

static double timediff(struct timeval*, struct timeval*);
static void print_size(double);

double timediff(struct timeval *bgn, struct timeval *end) {
    return (double) (end->tv_sec - bgn->tv_sec) + (end->tv_usec - bgn->tv_usec) * 1e-6;
}

static const char const *unit_lst[] = {"bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};

void print_size(double sz) {
    int i;
    for (i = 0; i < 9 && sz > 1024.0; ++i)
        sz /= 1024.0;
    printf("%f %s", sz, unit_lst[i]);
}

int main(int argc, char const *argv[])
{
    int pfd[2];                             /* Pipe file descriptors */
    char buf[BUF_SIZE];
    ssize_t total = TOTAL;
    ssize_t sz;
    struct timeval tv_bgn, tv_end;
    double speed;
    if (pipe(pfd) == -1)                    /* Create the pipe */
        errExit("pipe error");
    switch (fork()) {
    case -1:
        errExit("fork error");
    case 0:
        if (close(pfd[0]) == -1)
            errExit("Writer: close error");
        if (gettimeofday(&tv_bgn, NULL) == -1)
            errExit("Writer: gettimeofday error");
        for (;;) {
            sz = write(pfd[1], buf, BUF_SIZE);
            if (sz == -1)
                errExit("Writer: write error");
            total -= sz;
            if (total <= 0) break;
        }
        if (gettimeofday(&tv_end, NULL) == -1)
            errExit("Writer: gettimeofday error");
        if (close(pfd[1]) == -1)
            errExit("Writer: close error");
        speed = (double)(TOTAL) / timediff(&tv_bgn, &tv_end);
        printf("Writer speed: ");
        print_size(speed);
        printf("/sec\n");
        _exit(EXIT_SUCCESS);
    default:
        if (close(pfd[1]) == -1)
            errExit("Reader: close error");
        if (gettimeofday(&tv_bgn, NULL) == -1)
            errExit("Reader: gettimeofday error");
        while ((sz = read(pfd[0], buf, BUF_SIZE)) != 0) {
            if (sz == -1)
                errExit("Reader: read error");
            total -= sz;
            if (total <= 0) break;
        }
        if (gettimeofday(&tv_end, NULL) == -1)
            errExit("Reader: gettimeofday error");
        if (close(pfd[0]) == -1)
            errExit("Reader: close error");
        speed = (double)(TOTAL) / timediff(&tv_bgn, &tv_end);
        printf("Reader speed: ");
        print_size(speed);
        printf("/sec\n");
	if (wait(NULL) == -1)
	    errExit("Reader: wait error");
        break;
    }
    return 0;
}
