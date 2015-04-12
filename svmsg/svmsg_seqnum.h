#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

#define ID_PATH "/var/run/svmsg_seqnum.id"

struct request_msg {                    /* Requests (client to server) */
    long mtype;                         /* Should be 1 */
    long client_id;                     /* Identify of the Client */
};

struct response_msg {                   /* Responses (server to client) */
    long mtype;                         /* Should be client_id */
    unsigned long seqnum;               /* Sequence number / response message */
};
