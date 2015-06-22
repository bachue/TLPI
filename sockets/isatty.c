#include "tlpi_hdr.h"
#include <fcntl.h>
#include <termios.h>

int myisatty(int);

int myisatty(int files) {
    struct termios ts;
    return tcgetattr(files, &ts);
}

int main(int argc, char const *argv[])
{
    int fd;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s path\n", argv[0]);

    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
        errExit("open error");

    switch (myisatty(fd)) {
     case -1:
        if (errno == ENOTTY) {
           printf("no\n");
           exit(EXIT_FAILURE);
        } else {
           errExit("tcgetattr error");
        }
     default:
        printf("yes\n");
        exit(EXIT_SUCCESS);
     }

    return 0;
}
