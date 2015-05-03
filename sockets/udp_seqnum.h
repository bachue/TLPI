#include <sys/un.h>
#include <sys/socket.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 24
#define SERVER_SOCKET "/tmp/server.skt"
#define CLIENT_SOCKET "/tmp/client-%d.skt"

void log_err_exit(char*);
