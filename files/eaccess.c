#define _GNU_SOURCE
#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    char *path;
    uid_t ruid, euid, suid;

    if (argc != 2 || (argc > 1 && strcmp(argv[1], "--help") == 0))
        usageErr("%s file", argv[0]);

    path = argv[1];

    if (getresuid(&ruid, &euid, &suid) == -1)
        errExit("getresuid error");

    if (ruid != euid && setreuid(euid, euid) == -1)
        errExit("setruid error");

    access(path, R_OK) == 0 ? printf("r") : printf("-");
    access(path, W_OK) == 0 ? printf("w") : printf("-");
    access(path, X_OK) == 0 ? printf("x") : printf("-");
    printf("\n");

    if (ruid != euid && setreuid(ruid, euid) == -1)
        errExit("setruid error");

    return 0;
}
