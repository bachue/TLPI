#include "tlpi_hdr.h"
#include <sys/stat.h>

static void chmod_a_plus_rx(const char*);

int main(int argc, char  *argv[]) {
    int i;

    if (argc == 1 || (argc > 1 && strcmp(argv[1], "--help") == 0))
        usageErr("%s file [files ...]\n", argv[0]);

    for (i = 1; i < argc; i++)
        chmod_a_plus_rx(argv[i]);

    return 0;
}

void chmod_a_plus_rx(const char *path) {
    struct stat st;
    mode_t mode;
    if (stat(path, &st) == -1)
        errExit("stat error");
    mode = st.st_mode;
    if (S_ISREG(mode)) {
        mode |= S_IXUSR | S_IXGRP | S_IXOTH;
        if (chmod(path, mode) == -1)
            errExit("chmod error");
    }
}
