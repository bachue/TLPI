#include <mqueue.h>
#include <fcntl.h>              /* For definition of O_NONBLOCK */
#include <time.h>
#include "tlpi_hdr.h"

static void
usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-n] [-t timeout] mq-name\n", progName);
    fprintf(stderr, "    -n           Use O_NONBLOCK flag\n");
    fprintf(stderr, "    -t           Timeout (seconds)\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int flags, opt;
    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    struct timespec timeout, *timep = NULL;
    ssize_t numRead;

    flags = O_RDONLY;
    while ((opt = getopt(argc, argv, "nt:")) != -1) {
        switch (opt) {
        case 'n':
            flags |= O_NONBLOCK;
            break;
        case 't':
            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_sec += (time_t) atoi(optarg);
            timep = &timeout;
            break;
        default:
            usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t) -1)
        errExit("mq_open");

    /* We need to know the 'mq_msgsize' attribute of the queue in
       order to determine the size of the buffer for mq_receive() */

    if (mq_getattr(mqd, &attr) == -1)
        errExit("mq_getattr");

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL)
        errExit("malloc");

    numRead = mq_timedreceive(mqd, buffer, attr.mq_msgsize, &prio, timep);
    if (numRead == -1)
        errExit("mq_receive");

    printf("Read %ld bytes; priority = %u\n", (long) numRead, prio);
    if (write(STDOUT_FILENO, buffer, numRead) == -1)
        errExit("write");
    write(STDOUT_FILENO, "\n", 1);

    exit(EXIT_SUCCESS);
}
