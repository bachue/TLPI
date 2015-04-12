#include "svmsg_seqnum.h"
#include <signal.h>
#include <fcntl.h>

static void termsig_handler(int);
static void clear(void);
static void remove_msgkey(void);
static void delete_id_file(void);

static int server_id;

void termsig_handler(int sig) {
    clear();
    _exit(EXIT_SUCCESS);
}

void clear() {
    remove_msgkey();
    delete_id_file();
}

void remove_msgkey() {
    msgctl(server_id, IPC_RMID, NULL); // Ignore error
}

void delete_id_file() {
    unlink(ID_PATH); // Ignore error
}

int main(int argc, char const *argv[])
{
    int seqnum, svr_id_fd;
    struct request_msg req;
    struct response_msg resp;
    struct sigaction sa;

    server_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IWGRP);
    if (server_id == -1)
        errExit("msgget failed");
    if (atexit(clear) != 0)
        errExit("atexit error");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = termsig_handler;
    if (sigaction(SIGHUP, &sa, NULL) == -1) errExit("sigaction");
    if (sigaction(SIGINT, &sa, NULL) == -1) errExit("sigaction");
    if (sigaction(SIGTERM, &sa, NULL) == -1) errExit("sigaction");

    svr_id_fd = open(ID_PATH, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR, S_IWUSR);
    if (write(svr_id_fd, &server_id, sizeof(server_id)) == -1)
        errExit("write server id error");
    if (close(svr_id_fd) == -1)
        errExit("close server id error");

    for (seqnum = 0; ; seqnum++) {
        if (msgrcv(server_id, &req, sizeof(req), 1, 0) == -1)
            errExit("msgrcv error");
        resp.mtype = req.client_id;
        resp.seqnum = seqnum;
        if (msgsnd(server_id, &resp, sizeof(resp), 0) == -1)
            errExit("msgsnd error");
    }
    exit(EXIT_SUCCESS);
}
