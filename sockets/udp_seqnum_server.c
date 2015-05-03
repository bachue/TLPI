#include "udp_seqnum.h"
#include <signal.h>

static int daemonize(void);

int daemonize() {
    int maxfd, fd;

    switch (fork()) {
    case -1: log_err_exit("fork error");
    case 0: break;
    default: _exit(EXIT_SUCCESS);
    }

    if (setsid() == -1) return -1;

    switch (fork()) {
    case -1: log_err_exit("fork error");
    case 0: break;
    default: _exit(EXIT_SUCCESS);
    }

    umask(0);
    chdir("/");

    maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd == -1) maxfd = 8192;

    for (fd = 0; fd < maxfd; fd++)
        close(fd);

    close(STDIN_FILENO);
    fd = open("/dev/null", O_RDWR);
    if (fd != STDIN_FILENO)
        return -1;
    if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
        return -1;
    if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
        return -1;
    return 0;
}

int main(int argc, char const *argv[])
{
    int sfd, seqnum;
    socklen_t len;
    char buf[BUF_SIZE];
    struct sockaddr_un svaddr, claddr;

    openlog("udp_seqnum_server", LOG_PID, LOG_USER);

    if (daemonize() == -1)
        log_err_exit("daemonize error");

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        log_err_exit("socket error");

    errno = 0;
    if (remove(SERVER_SOCKET) == -1 && errno != ENOENT)
        log_err_exit("remove socket file error");

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SERVER_SOCKET, sizeof(svaddr.sun_path));

    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1)
        log_err_exit("bind server error");

    for (seqnum = 0; ; seqnum++) {
        len = sizeof(struct sockaddr_un);
        if (recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &claddr, &len) == -1)
            log_err_exit("recvfrom error");
        snprintf(buf, BUF_SIZE - 1, "%d", seqnum);
        buf[BUF_SIZE - 1] = '\0';
        if (sendto(sfd, buf, strlen(buf), 0, (struct sockaddr *) &claddr, len) != strlen(buf))
            log_err_exit("sendto error");
    }

    exit(EXIT_SUCCESS);
}
