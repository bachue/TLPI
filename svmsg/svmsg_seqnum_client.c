#include "svmsg_seqnum.h"
#include <time.h>

static void log_err_exit(char*);

void log_err_exit(char *log) {
    char strerr[1024];
    strerror_r(errno, strerr, 1023);
    strerr[1023] = '\0';
    syslog(LOG_ERR, "[%s] %s", strerr, log);
    errExit(log);
}

int main(int argc, char const *argv[]) {
    int server_id, svr_id_fd;
    long client_id;
    struct request_msg req;
    struct response_msg resp;

    openlog("svmsg_seqnum_client", LOG_PID, LOG_USER);

    srand(time(NULL));
    client_id = rand() % (RAND_MAX - 1) + 1;
    syslog(LOG_DEBUG, "generate client_id = %ld", client_id);

    svr_id_fd = open(ID_PATH, O_RDONLY);
    syslog(LOG_DEBUG, "open %s, fileid = %d", ID_PATH, svr_id_fd);
    if (read(svr_id_fd, &server_id, sizeof(server_id)) == -1)
        log_err_exit("read server id error");
    syslog(LOG_DEBUG, "read from %s, server_id = %d", ID_PATH, server_id);
    if (close(svr_id_fd) == -1)
        log_err_exit("close server id error");

    req.mtype = 1;
    req.client_id = client_id;
    if (msgsnd(server_id, &req, sizeof(req), 0) == -1)
        log_err_exit("msgsnd error");
    syslog(LOG_INFO, "request sent mtype = %d client_id = %ld", 1, req.client_id);
    if (msgrcv(server_id, &resp, sizeof(resp), client_id, 0) == -1)
        log_err_exit("msgrcv error");
    syslog(LOG_INFO, "response received, seqnum = %ld\n", resp.seqnum);
    printf("%ld\n", resp.seqnum);

    closelog();

    exit(EXIT_SUCCESS);
}
