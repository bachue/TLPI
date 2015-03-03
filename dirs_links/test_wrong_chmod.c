#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char  *argv[]) {
    if (mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR) == -1)
        errExit("mkdir error");
    if (chdir("test") == -1)
        errExit("chdir error");
    if (open("myfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR) == -1)
        errExit("open error");
    if (symlink("myfile", "../mylink") == -1)
        errExit("symlink error");
    if (chmod("../mylink", S_IRUSR) == -1)
        errExit("chmod error");
    return 0;
}
