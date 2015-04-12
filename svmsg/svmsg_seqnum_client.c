#include "svmsg_seqnum.h"
#include <time.h>
#include <fcntl.h>

int main(int argc, char const *argv[]) {
    int server_id, svr_id_fd;
    long client_id;
    struct request_msg req;
    struct response_msg resp;

    srand(time(NULL));
    client_id = rand() % (RAND_MAX - 1) + 1;

    svr_id_fd = open(ID_PATH, O_RDONLY);
    if (read(svr_id_fd, &server_id, sizeof(server_id)) == -1)
        errExit("read server id error");
    if (close(svr_id_fd) == -1)
        errExit("close server id error");

    req.mtype = 1;
    req.client_id = client_id;
    if (msgsnd(server_id, &req, sizeof(req), 0) == -1)
        errExit("msgsnd error");
    if (msgrcv(server_id, &resp, sizeof(resp), client_id, 0) == -1)
        errExit("msgrcv error");
    printf("%ld\n", resp.seqnum);

    exit(EXIT_SUCCESS);
}
