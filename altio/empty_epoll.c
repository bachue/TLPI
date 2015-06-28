#include <sys/epoll.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char const *argv[])
{
    int epfd, ready;
    epfd = epoll_create(0);
    if (epfd == -1)
        errExit("epoll_create error");
    for (;;) {
        ready = epoll_wait(epfd, NULL, 0, -1);
        if (ready == -1) {
            if (errno == EINTR)
                return 0;
            else
                errExit("epoll_wait");
        }
    }
    printf("Ready: %d\n", ready);
    exit(EXIT_SUCCESS);
}
