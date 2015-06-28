#include <sys/time.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include "tlpi_hdr.h"

static int pfd[2];                      /* File descriptors for pipe */

static void
handler(int sig)
{
    int savedErrno;                     /* In case we change 'errno' */

    savedErrno = errno;
    if (write(pfd[1], "x", 1) == -1 && errno != EAGAIN)
        errExit("write");
    errno = savedErrno;
}

int
main(int argc, char *argv[])
{
    struct pollfd *pollFd;
    int ready, flags;
    struct sigaction sa;
    char ch;
    int fd, j, timeout;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s {timeout|-} fd...\n"
                "\t\t('-' means infinite timeout)\n", argv[0]);

    /* Initialize 'timeout', 'readfds', and 'nfds' for select() */

    if (strcmp(argv[1], "-") == 0) {
        timeout = -1;
    } else {
        timeout = getLong(argv[1], 0, "timeout") * 1e3;
    }

    /* Build the 'readfds' from the fd numbers given in command line */

    pollFd = calloc(argc - 1, sizeof(struct pollfd));
    for (j = 2; j < argc; j++) {
        fd = getInt(argv[j], 0, "fd");
        if (fd >= FD_SETSIZE)
            cmdLineErr("file descriptor exceeds limit (%d)\n", FD_SETSIZE);

        pollFd[j - 1].fd = fd;
        pollFd[j - 1].events = POLLIN;
    }

    /* Create pipe before establishing signal handler to prevent race */

    if (pipe(pfd) == -1)
        errExit("pipe");

    pollFd[0].fd = pfd[0];
    pollFd[0].events = POLLIN;

    /* Make read and write ends of pipe nonblocking */

    flags = fcntl(pfd[0], F_GETFL);
    if (flags == -1)
        errExit("fcntl-F_GETFL");
    flags |= O_NONBLOCK;                /* Make read end nonblocking */
    if (fcntl(pfd[0], F_SETFL, flags) == -1)
        errExit("fcntl-F_SETFL");

    flags = fcntl(pfd[1], F_GETFL);
    if (flags == -1)
        errExit("fcntl-F_GETFL");
    flags |= O_NONBLOCK;                /* Make write end nonblocking */
    if (fcntl(pfd[1], F_SETFL, flags) == -1)
        errExit("fcntl-F_SETFL");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;           /* Restart interrupted reads()s */
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        errExit("sigaction");

    while ((ready = poll(pollFd, argc - 1, timeout)) == -1 &&
            errno == EINTR)
        continue;                       /* Restart if interrupted by signal */
    if (ready == -1)                    /* Unexpected error */
        errExit("select");

    if (pollFd[0].revents & POLLIN) {   /* Handler was called */
        printf("A signal was caught\n");

        for (;;) {                      /* Consume bytes from pipe */
            if (read(pfd[0], &ch, 1) == -1) {
                if (errno == EAGAIN)
                    break;              /* No more bytes */
                else
                    errExit("read");    /* Some other error */
            }

            /* Perform any actions that should be taken in response to signal */
        }
    }

    /* Examine file descriptor sets returned by select() to see
       which other file descriptors are ready */

    printf("ready = %d\n", ready);
    for (j = 2; j < argc; j++) {
        fd = getInt(argv[j], 0, "fd");
        printf("%d: %s\n", fd, pollFd[j - 1].revents & POLLIN ? "r" : "");
    }

    /* And check if read end of pipe is ready */

    printf("%d: %s   (read end of pipe)\n", pfd[0],
            pollFd[0].revents & POLLIN ? "r" : "");

    if (timeout >= 0)
        printf("timeout after select(): %d\n", timeout / 1000);

    exit(EXIT_SUCCESS);
}
