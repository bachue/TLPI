#include "mq_seqnum.h"

static void clear(void);

static int client_id;
static char client_mq[22];

void clear() {
    mq_unlink(client_mq);
    closelog();
}

int main(int argc, char const *argv[])
{
    int seqnum;
    mqd_t mqd, client_mqd;
    struct mq_attr attr;

    openlog("mq_seqnum_client", LOG_PID, LOG_USER);

    srand(time(NULL));
    client_id = rand();
    snprintf(client_mq, 21, CLIENT_MQ, client_id);

    client_mqd = mq_open(client_mq, O_CREAT | O_RDONLY, S_IWUSR | S_IRUSR, NULL);
    if (client_mqd == (mqd_t) -1)
        log_err_exit("mq_open server error");
    if (atexit(clear) != 0)
        log_err_exit("atexit error");
    syslog(LOG_INFO, "open mq %s to receive", client_mq);

    if (mq_getattr(client_mqd, &attr) == -1)
        errExit("mq_getattr");

    mqd = mq_open(SERVER_MQ, O_WRONLY);
    if (mqd == (mqd_t) -1)
        log_err_exit("mq_open client error");
    syslog(LOG_INFO, "open mq %s to send", SERVER_MQ);

    if (mq_send(mqd, (char *) &client_id, sizeof(client_id), 0) == -1)
        log_err_exit("mq_send error");
    syslog(LOG_INFO, "client id sent: %d", client_id);

    if (mq_close(mqd) == -1)
        log_err_exit("mq_close server error");

    if (mq_receive(client_mqd, (char *) &seqnum, attr.mq_msgsize, NULL) == -1)
        log_err_exit("mq_receive error");

    if (mq_close(client_mqd) == -1)
        log_err_exit("mq_close client error");

    printf("%d\n", seqnum);

    exit(EXIT_SUCCESS);
}
