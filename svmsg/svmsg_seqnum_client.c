#include "svmsg_seqnum.h"

static int client_id;

static void remove_msgkey(void)
{
    if (msgctl(client_id, IPC_RMID, NULL) == -1)
        errExit("msgctl");
}

int main(int argc, char const *argv[]) {
    int server_id;
    struct request_msg req;
    struct response_msg resp;
    server_id = msgget(SERVER_KEY, S_IWUSR);
    if (server_id == -1)
        errExit("server_id error");

    if (atexit(remove_msgkey) != 0)
        errExit("atexit error");

    client_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IRGRP);
    if (client_id == -1)
        errExit("client_id error");
    req.client_id = client_id;
    if (msgsnd(server_id, &req, sizeof(req), 0) == -1)
        errExit("msgsnd error");
    if (msgrcv(client_id, &resp, sizeof(resp), 0, 0) == -1)
        errExit("msgrcv error");
    printf("%ld\n", resp.seqnum);

    exit(EXIT_SUCCESS);
}
