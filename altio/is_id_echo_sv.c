#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "inet_sockets.h"       /* Declares our socket functions */
#include "tlpi_hdr.h"

#define SERVICE "echo"
#define BUF_SIZE 1024

static void grimReaper(int);
static void startWorker(int);
static void handleRequest(int);

void grimReaper(int sig) {
    int savedErrno;             /* Save 'errno' in case changed here */
    savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

void startWorker(int cfd) {
    printf("start worker for %d\n", cfd);
    switch (fork()) {
    case -1: errExit("fork error");
    case 0:
        handleRequest(cfd);
        _exit(EXIT_SUCCESS);
    default:
        if (close(cfd) == -1)
            errExit("close error");
    }
}

void handleRequest(int cfd) {
    char buf[BUF_SIZE];
    ssize_t numRead;

    while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
        if (write(cfd, buf, numRead) != numRead)
            errExit("write error");
    }

    if (numRead == -1) errExit("read error");
}

int main(int argc, char const *argv[]) {
    int udpsockfd, tcpsockfd, sockfd, epfd, ready, i, closedNum;
    struct epoll_event ev;
    struct epoll_event evlist[2];
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction error");

    udpsockfd = inetBind(SERVICE, SOCK_DGRAM, NULL);
    if (udpsockfd == -1)
        errExit("inetBind error");

    tcpsockfd = inetListen(SERVICE, 10, NULL);
    if (tcpsockfd == -1)
        errExit("inetListen error");

    epfd = epoll_create(2);
    if (epfd == -1)
        errExit("epoll_create error");

    ev.events = EPOLLIN;
    ev.data.fd = udpsockfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, udpsockfd, &ev) == -1)
        errExit("epoll_ctl");

    ev.events = EPOLLIN;
    ev.data.fd = tcpsockfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, tcpsockfd, &ev) == -1)
        errExit("epoll_ctl");

    for (closedNum = 0; closedNum < 2;) {
        ready = epoll_wait(epfd, evlist, 2, -1);
        if (ready == -1) {
            if (errno == EINTR)
                continue;               /* Restart if interrupted by signal */
            else
                errExit("epoll_wait error");
        }

        for (i = 0; i < ready; ++i) {
            if (evlist[i].events & EPOLLIN) {
                printf("event from %d\n", evlist[i].data.fd);
                if (evlist[i].data.fd == tcpsockfd) {
                    printf("tcp data from %d\n", tcpsockfd);
                    sockfd = accept(tcpsockfd, NULL, NULL);
                    if (sockfd == -1)
                        errExit("accept error");
                    startWorker(sockfd);
                } else if (evlist[i].data.fd == udpsockfd) {
                    printf("udp data from %d\n", udpsockfd);
                    startWorker(udpsockfd);
                } else
                    fatal("unknown event fd: %d\n", evlist[i].data.fd);
            } else if (evlist[i].events & (EPOLLHUP | EPOLLERR)) {
                if (close(evlist[i].data.fd) == -1)
                    errExit("close");
                closedNum++;
            }
        }
    }

    return 0;
}
