#include "tlpi_hdr.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

static void addrstr(int ai_family, struct sockaddr *addr, char* addrbuf) {
    switch (ai_family) {
    case AF_INET:
        if (inet_ntop(ai_family, &((struct sockaddr_in *) addr)->sin_addr, addrbuf, INET_ADDRSTRLEN) == NULL)
            errExit("inet_ntop");
        break;
    case AF_INET6:
        if (inet_ntop(ai_family, &((struct sockaddr_in6 *) addr)->sin6_addr, addrbuf, INET6_ADDRSTRLEN) == NULL)
            errExit("inet_ntop");
        break;
    default:
        fatal("Unknown ai_family");
    }
}

int main(int argc, char const *argv[])
{
    int s;
    struct addrinfo hints, *result, *rp;
    char addrbuf[max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s host [service]\n", argv[0]);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;        /* Allows IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0)
        fatal(gai_strerror(s));

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        addrstr(rp->ai_family, rp->ai_addr, addrbuf);
        printf("%s\n", addrbuf);
    }

    return 0;
}
