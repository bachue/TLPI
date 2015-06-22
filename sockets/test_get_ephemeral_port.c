#include <sys/socket.h>
#include <arpa/inet.h>
#include "tlpi_hdr.h"

int main(int argc, char const *argv[])
{
    int sfd;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
        errExit("socket error");

    if (listen(sfd, 16) == -1)
        errExit("listen error");

    if (getsockname(sfd, (struct sockaddr *) &addr, &len) == -1)
        errExit("getsockname error");

    printf("%d\n", addr.sin_port);

    return 0;
}
