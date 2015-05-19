#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include "inet_sockets.h"       /* Declares our socket functions */
#include "tlpi_hdr.h"

#define ADDRSTR_LEN 4096
#define BUFFER_SIZE 4096

static int to_connect() {
    int sfd;
    sfd = inetConnect("localhost", "5000", SOCK_STREAM);
    if (sfd == -1) errExit("inetConnect error");
    return sfd;
}

int main(int argc, char const *argv[])
{
    const char *arg1, *arg2;
    int sfd;
    ssize_t got;
    char buffer[BUFFER_SIZE];

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s cmd [options ...]\n", argv[0]);

    if (strcmp(argv[1], "get") == 0) {
        if (argc != 3)
            usageErr("%s get key\n", argv[0]);
        arg1 = argv[2];
        sfd = to_connect();
        snprintf(buffer, BUFFER_SIZE - 1, "get %s", arg1);
        buffer[BUFFER_SIZE - 1] = '\0';
        if (write(sfd, buffer, strlen(buffer)) != strlen(buffer))
            errExit("write error");
        got = read(sfd, buffer, BUFFER_SIZE - 1);
        if (got < 0)
            errExit("read error");
        else if (got == 0)
            return 0;
        buffer[got] = '\0';
        printf("%s\n", buffer);
    } if (strcmp(argv[1], "set") == 0) {
        if (argc != 4)
            usageErr("%s set key value\n", argv[0]);
        arg1 = argv[2];
        arg2 = argv[3];
        sfd = to_connect();
        snprintf(buffer, BUFFER_SIZE - 1, "set %s %s", arg1, arg2);
        buffer[BUFFER_SIZE - 1] = '\0';
        if (write(sfd, buffer, strlen(buffer)) != strlen(buffer))
            errExit("write error");
    } if (strcmp(argv[1], "del") == 0) {
        if (argc != 3)
            usageErr("%s del key\n", argv[0]);
        arg1 = argv[2];
        sfd = to_connect();
        snprintf(buffer, BUFFER_SIZE - 1, "del %s", arg1);
        buffer[BUFFER_SIZE - 1] = '\0';
        if (write(sfd, buffer, strlen(buffer)) != strlen(buffer))
            errExit("write error");
    } if (strcmp(argv[1], "list") == 0) {
        if (argc != 2)
            usageErr("%s list\n", argv[0]);
        sfd = to_connect();
        if (write(sfd, "list", strlen("list")) != strlen("list"))
            errExit("write error");
        got = read(sfd, buffer, BUFFER_SIZE - 1);
        if (got < 0)
            errExit("read error");
        else if (got == 0)
            return 0;
        buffer[got] = '\0';
        printf("%s", buffer);
    }

    return 0;
}
