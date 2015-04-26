#include "mq_seqnum.h"
#include <signal.h>

#define NOTIFY_SIG SIGUSR1

static void notify_handler(int);
static void termsig_handler(int);
static int daemonize(void);
static void clear(void);

void notify_handler(int sig) {
    syslog(LOG_INFO, "signal %s caught, ready to receive from mq", strsignal(sig));
}

void termsig_handler(int sig) {
    syslog(LOG_INFO, "signal %s caught, ready to terminate", strsignal(sig));
    clear();
    _exit(EXIT_SUCCESS);
}

void clear() {
    mq_unlink(SERVER_MQ);
    closelog();
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
    int seqnum = 0, client_id;
    mqd_t mqd, client_mqd;
    char client_mq[22];
    struct sigaction sa;
    struct sigevent sev;
    struct mq_attr attr;

    openlog("mq_seqnum_server", LOG_PID, LOG_USER);

    if (daemonize() == -1)
        log_err_exit("daemonize error");

    mqd = mq_open(SERVER_MQ, O_CREAT | O_RDONLY | O_NONBLOCK, S_IWUSR | S_IRUSR, NULL);
    if (mqd == (mqd_t) -1)
        log_err_exit("mq_open server error");
    if (atexit(clear) != 0)
        log_err_exit("atexit error");

    syslog(LOG_INFO, "open mq %s to receive", SERVER_MQ);

    if (mq_getattr(mqd, &attr) == -1)
        errExit("mq_getattr");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = termsig_handler;
    if (sigaction(SIGHUP, &sa, NULL) == -1) log_err_exit("sigaction error");
    if (sigaction(SIGINT, &sa, NULL) == -1) log_err_exit("sigaction error");
    if (sigaction(SIGTERM, &sa, NULL) == -1) log_err_exit("sigaction error");

    sigaddset(&sa.sa_mask, NOTIFY_SIG);
    if (sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL) == -1)
        errExit("sigprocmask");

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = notify_handler;
    if (sigaction(NOTIFY_SIG, &sa, NULL) == -1) log_err_exit("sigaction error");

    syslog(LOG_DEBUG, "signal handler setup");

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    if (mq_notify(mqd, &sev) == -1) log_err_exit("mq_notify setup error");

    syslog(LOG_DEBUG, "mq_notify setup");

    sigemptyset(&sa.sa_mask);

    for (;;) {
        sigsuspend(&sa.sa_mask);
        if (mq_notify(mqd, &sev) == -1)
            errExit("mq_notify resetup error");

        syslog(LOG_DEBUG, "mq_notify resetup");

        while (mq_receive(mqd, (char *) &client_id, attr.mq_msgsize, NULL) >= 0) {
            syslog(LOG_INFO, "Receive request from client %d", client_id);
            snprintf(client_mq, 21, CLIENT_MQ, client_id);
            client_mq[21] = '\0';
            client_mqd = mq_open(client_mq, O_WRONLY);
            if (client_mqd == (mqd_t) -1)
                log_err_exit("mq_open client error");
            syslog(LOG_INFO, "open mq %s to send", client_mq);
            seqnum ++;
            if (mq_send(client_mqd, (char *) &seqnum, sizeof(seqnum), 0) == -1)
                log_err_exit("mq_send error");
            syslog(LOG_INFO, "sequence number sent: %d", seqnum);
            if (mq_close(client_mqd) == -1)
                log_err_exit("mq_close client error");
        }

        if (errno != EAGAIN)            /* Unexpected error */
            log_err_exit("mq_receive error");
    }

    if (mq_close(mqd) == -1)
        log_err_exit("mq_close server error");
    exit(EXIT_SUCCESS);
}
