#include "udp_seqnum.h"

int main(int argc, char const *argv[])
{
    struct sockaddr_un svaddr, claddr;
    int sfd, readNum;
    char resp[BUF_SIZE];

    openlog("mq_seqnum_client", LOG_PID, LOG_USER);

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        log_err_exit("socket error");

    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path, BUF_SIZE, CLIENT_SOCKET, getpid());

    syslog(LOG_INFO, "Bind: %s", claddr.sun_path);

    if (bind(sfd, (struct sockaddr *) &claddr, sizeof(struct sockaddr_un)) == -1)
        log_err_exit("bind client error");

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SERVER_SOCKET, sizeof(svaddr.sun_path) - 1);

    if (sendto(sfd, NULL, 0, 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) != 0)
        log_err_exit("sendto error");

    if ((readNum = read(sfd, resp, BUF_SIZE)) == -1)
        log_err_exit("read error");

    resp[readNum] = '\0';
    printf("%s\n", resp);

    remove(claddr.sun_path);
    exit(EXIT_SUCCESS);
}
