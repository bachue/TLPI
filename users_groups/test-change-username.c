#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    uid_t uid = 0;
    char *root_name;
    root_name = userNameFromId(uid);
    strcpy(root_name, "rot");
    printf("root_name = %s\n", root_name);
    root_name = userNameFromId(uid);
    printf("root_name = %s\n", root_name);
    exit(EXIT_SUCCESS);
}
