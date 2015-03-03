#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    char *path = argv[1];
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);
    if (remove(path) == -1)
        errExit("remove error");
    return 0;
}
