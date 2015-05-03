#include "udp_seqnum.h"

void log_err_exit(char *log) {
    char strerr[1024];
    strerror_r(errno, strerr, 1023);
    strerr[1023] = '\0';
    syslog(LOG_ERR, "[%s] %s", strerr, log);
    errExit(log);
}
