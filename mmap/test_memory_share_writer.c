#include "test_memory_share.h"

int main(int argc, char const *argv[])
{
    int reqfd, respfd, sharefd;
    long bytes, xfrs;
    char *buf;
    ssize_t readnum;
    size_t to_read;
    if (mkfifo(REQ_PATH, S_IRUSR | S_IWUSR) == -1) {
        if (errno != EEXIST)
            errExit("req mkfifo error");
    }
    if (mkfifo(RESP_PATH, S_IRUSR | S_IWUSR) == -1) {
        if (errno != EEXIST)
            errExit("resp mkfifo error");
    }
    reqfd = open(REQ_PATH, O_WRONLY | O_CLOEXEC);
    if (reqfd == -1)
        errExit("open req fifo error");

    respfd = open(RESP_PATH, O_RDONLY | O_CLOEXEC);
    if (respfd == -1)
        errExit("open resp fifo error");

    sharefd = open(SHARED_MEM_PATH, O_RDWR | O_CREAT | O_TRUNC | O_EXCL, S_IRUSR | S_IWUSR);
    if (sharefd == -1)
        errExit("open shared file error");
    if (ftruncate(sharefd, BUF_SIZE) == -1)
        errExit("expand shared file error");

    buf = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharefd, 0);
    if (buf == MAP_FAILED)
        errExit("mmap error");

    for (xfrs = bytes = 0; ; xfrs++, bytes += to_read) {
        readnum = read(STDIN_FILENO, buf, BUF_SIZE);
        if (readnum == -1)
            errExit("read from STDIN error");
        to_read = (size_t) readnum;

        if (write(reqfd, &to_read, sizeof(to_read)) == -1)
            errExit("write to req fifo error");
        if (read(respfd, buf, 1) == -1)
            errExit("read response from resp fifo error");
        if (to_read == 0)
            break;
    }

    fprintf(stderr, "Sent %ld bytes (%ld xfrs)\n", bytes, xfrs);

    if (munmap(buf, BUF_SIZE) == -1)
        errExit("mummap error");

    if (unlink(SHARED_MEM_PATH) == -1)
        errExit("delete shared file error");
    if (close(sharefd) == -1)
        errExit("close shared file error");
    if (close(reqfd) == -1)
        errExit("close req fifo error");
    if (close(respfd) == -1)
        errExit("close resp fifo error");
    return 0;
}
