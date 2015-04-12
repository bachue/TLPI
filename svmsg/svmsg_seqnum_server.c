#include "svmsg_seqnum.h"
#include <signal.h>

static void termsig_handler(int);
static void clear(void);
static void remove_msgkey(void);
static void delete_id_file(void);
static void log_err_exit(char*);
static int daemonize(void);

static int server_id;

void termsig_handler(int sig) {
    syslog(LOG_INFO, "signal %s caught, ready to terminate", strsignal(sig));
    clear();
    _exit(EXIT_SUCCESS);
}

void clear() {
    syslog(LOG_INFO, "ready to clear runtime environment");
    remove_msgkey();
    delete_id_file();
    closelog();
}

void remove_msgkey() {
    msgctl(server_id, IPC_RMID, NULL); // Ignore error
}

void delete_id_file() {
    unlink(ID_PATH); // Ignore error
}

void log_err_exit(char *log) {
    char strerr[1024];
    strerror_r(errno, strerr, 1023);
    strerr[1023] = '\0';
    syslog(LOG_ERR, "[%s] %s", strerr, log);
    errExit(log);
}

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
    int seqnum, svr_id_fd;
    struct request_msg req;
    struct response_msg resp;
    struct sigaction sa;

    openlog("svmsg_seqnum_server", LOG_PID, LOG_USER);

    server_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IWGRP);
    if (server_id == -1)
        log_err_exit("msgget failed");
    if (atexit(clear) != 0)
        log_err_exit("atexit error");

    syslog(LOG_DEBUG, "msgget returns %d", server_id);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = termsig_handler;
    if (sigaction(SIGHUP, &sa, NULL) == -1) log_err_exit("sigaction");
    if (sigaction(SIGINT, &sa, NULL) == -1) log_err_exit("sigaction");
    if (sigaction(SIGTERM, &sa, NULL) == -1) log_err_exit("sigaction");

    syslog(LOG_DEBUG, "signal handler setup");

    svr_id_fd = open(ID_PATH, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR, S_IWUSR);
    syslog(LOG_DEBUG, "open %s, fileid = %d", ID_PATH, svr_id_fd);
    if (write(svr_id_fd, &server_id, sizeof(server_id)) == -1)
        log_err_exit("write server id error");
    syslog(LOG_DEBUG, "write into %s, server_id = %d", ID_PATH, server_id);
    if (close(svr_id_fd) == -1)
        log_err_exit("close server id error");

    if (daemonize() == -1)
        log_err_exit("daemonize error");

    for (seqnum = 0; ; seqnum++) {
        syslog(LOG_DEBUG, "waiting for queue %d, type = %d", server_id, 1);
        if (msgrcv(server_id, &req, sizeof(req), 1, 0) == -1)
            log_err_exit("msgrcv error");
        syslog(LOG_INFO, "request received client_id = %ld", req.client_id);
        resp.mtype = req.client_id;
        resp.seqnum = seqnum;
        if (msgsnd(server_id, &resp, sizeof(resp), 0) == -1)
            log_err_exit("msgsnd error");
        syslog(LOG_INFO, "response sent to client_id = %ld, seqnum = %d", req.client_id, seqnum);
    }
    exit(EXIT_SUCCESS);
}
