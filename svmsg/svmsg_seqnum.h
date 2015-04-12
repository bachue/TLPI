#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

#define SERVER_KEY 0x2aabbaa2           /* Key for server's message queue */

struct request_msg {                    /* Requests (client to server) */
    long mtype;                         /* Unused */
    int  client_id;                     /* ID of client's message queue */
};

struct response_msg {                   /* Responses (server to client) */
    long mtype;                         /* Unused */
    unsigned long seqnum;               /* Sequence number / response message */
};
