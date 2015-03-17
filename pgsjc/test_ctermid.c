#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    char buf[L_ctermid + 1], *ptr;
    ptr = ctermid(buf);
    buf[L_ctermid] = '\0';
    printf("ctermid: %s\n", ptr);
    return 0;
}
