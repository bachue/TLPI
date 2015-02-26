#include "tlpi_hdr.h"
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <stddef.h>

void create_files(const char*);
void delete_files(const char*);

int main(int argc, char  *argv[]) {
    int num, i;
    char *path;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s num path\n", argv[0]);

    num = getInt(argv[1], GN_GT_0, "num of files");
    path = argv[2];

    if (num >= 1000000000)
        fatal("num is too large");

    srand(time(NULL));

    for (i = 0; i < num; ++i)
        create_files(path);
    delete_files(path);

    return 0;
}

void create_files(const char *root) {
    char path[PATH_MAX];
    int fd;
    ssize_t written;
retry:
    snprintf(path, PATH_MAX, "%s/test.%.9d", root, rand() % 1000000000);
    errno = 0;
    fd = open(path, O_WRONLY | O_CREAT | O_EXCL);
    if (fd == -1) {
        if (errno == EEXIST)
            goto retry;
        else
            errExit("open");
    }
    written = write(fd, "1", 1);
    if (written < 0)
        errExit("read");
    else if (written != 1)
        fatal("cannot write all");
    if (close(fd) == -1)
        errExit("close");
}

void delete_files(const char *root) {
    struct dirent *pdirent, *pdirentbuf;
    DIR *dir = opendir(root);
    const size_t dirent_size = offsetof(struct dirent, d_name) + fpathconf(dirfd(dir), _PC_NAME_MAX) + 1;
    char path[PATH_MAX];

    if (dir == NULL)
        errExit("opendir");

    pdirentbuf = (struct dirent*) malloc(dirent_size);
    if (pdirentbuf == NULL)
        errExit("malloc");

    for (;;) {
        readdir_r(dir, pdirentbuf, &pdirent);
        if (pdirent == NULL)
            break;
        if (strstr(pdirent->d_name, "test.") != pdirent->d_name)
            continue;
        snprintf(path, PATH_MAX, "%s/%s", root, pdirent->d_name);
        if (unlink(path) == -1)
            errExit("unlink");
    }

    free(pdirentbuf);
}
