#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    int fd;
    struct stat st;
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s path\n", argv[0]);

    fd = open(argv[1], 0);
    if (fd == -1)
        errExit("open error");
    if (fstat(fd, &st) == -1)
        errExit("fstat error");

    printf("major = %u, minor = %u\n", major(st.st_rdev), minor(st.st_rdev));
    return 0;
}
