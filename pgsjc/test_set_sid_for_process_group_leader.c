#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    if (getpgrp() == getpid()) {
        printf("Sid: %ld\n", (long) getsid(0));
        if (setsid() == -1)
            errExit("setsid error (expected)");
        fprintf(stderr, "Error: it's impossible!\n");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "Error: only test for process group leader\n");
        exit(EXIT_FAILURE);
    }
}
