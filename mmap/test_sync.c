#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define MEM_SIZE 10

void write_something(char*, char*, size_t);

void write_something(char *path, char *content, size_t size) {
    int fd;
    fd = open(path, O_WRONLY);
    if (fd == -1)
        errExit("open error");
    if (write(fd, content, size) == -1)
        errExit("write error");
    if (close(fd) == -1)
        errExit("close error");
}

int
main(int argc, char *argv[])
{
    char *addr;
    int fd;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file new-value\n", argv[0]);

    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        errExit("open error");

    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        errExit("mmap error");

    if (close(fd) == -1)                /* No longer need 'fd' */
        errExit("close error");

    printf("Current string=%.*s\n", MEM_SIZE, addr);

    switch (fork()) {
    case -1: errExit("fork error");
    case 0:
        write_something(argv[0], argv[1], sizeof(argv[1]));
        _exit(EXIT_SUCCESS);
    default:
        if (wait(NULL) == -1)
            errExit("wait error");
        break;
    }

    printf("Current string=%.*s\n", MEM_SIZE, addr);

    exit(EXIT_SUCCESS);
}
