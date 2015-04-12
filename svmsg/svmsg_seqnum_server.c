#include "svmsg_seqnum.h"

static int server_id;

static void remove_msgkey(void)
{
    if (msgctl(server_id, IPC_RMID, NULL) == -1)
        errExit("remove msgqueue error");
}

int main(int argc, char const *argv[])
{
    int seqnum;
    struct request_msg req;
    struct response_msg resp;
    server_id = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IWGRP);
    if (atexit(remove_msgkey) != 0)
        errExit("atexit error");
    if (server_id == -1)
        errExit("msgget failed");

    for (seqnum = 0; ; seqnum++) {
        if (msgrcv(server_id, &req, sizeof(req), 1, 0) == -1)
            errExit("msgrcv error");
        resp.mtype = req.client_id;
        resp.seqnum = seqnum;
        if (msgsnd(server_id, &resp, sizeof(resp), 0) == -1)
            errExit("msgsnd error");
    }
    exit(EXIT_SUCCESS);
}
