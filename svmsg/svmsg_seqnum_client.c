#include "svmsg_seqnum.h"
#include <time.h>

int main(int argc, char const *argv[]) {
    int server_id;
    long client_id;
    struct request_msg req;
    struct response_msg resp;

    srand(time(NULL));
    client_id = rand() % (RAND_MAX - 1) + 1;

    server_id = msgget(SERVER_KEY, S_IWUSR);
    if (server_id == -1)
        errExit("server_id error");

    req.mtype = 1;
    req.client_id = client_id;
    if (msgsnd(server_id, &req, sizeof(req), 0) == -1)
        errExit("msgsnd error");
    if (msgrcv(server_id, &resp, sizeof(resp), client_id, 0) == -1)
        errExit("msgrcv error");
    printf("%ld\n", resp.seqnum);

    exit(EXIT_SUCCESS);
}
