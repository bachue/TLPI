#include <mqueue.h>
#include <syslog.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

#define SERVER_MQ "/server-mq"
#define CLIENT_MQ "/client-%d-mq"

void log_err_exit(char*);
