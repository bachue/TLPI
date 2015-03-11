#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include "tlpi_hdr.h"

static void print_fdlist(const pid_t pid) {
    char dirpath[PATH_MAX + 1];
    char linkpath[PATH_MAX + 1];
    DIR *dir;
    struct dirent entry, *dp;
    ssize_t size;
    snprintf(dirpath, PATH_MAX, "/proc/%ld/fd", (long) pid);
    dirpath[PATH_MAX] = '\0';
    dir = opendir(dirpath);
    if (dir == NULL)
        errExit("opendir error");
    for (;;) {
        readdir_r(dir, &entry, &dp);
        if (dp == NULL)
            break;
        if (strcmp(entry.d_name, ".") == 0 ||
            strcmp(entry.d_name, "..") == 0)
            continue;
        size = readlinkat(dirfd(dir), entry.d_name, linkpath, PATH_MAX);
        if (size == -1)
            errExit("readlinkat error");
        linkpath[size] = '\0';
        printf("%s --> %s\n", entry.d_name, linkpath);
    }
    closedir(dir);
}

int
main(int argc, char *argv[])
{
    int flags1, flags2, fd1, fd2, fd3;
    pid_t pid;

    char template1[] = "/tmp/test.XXXXXX";
    char template2[] = "/tmp/test.XXXXXX";
    char template3[] = "/tmp/test.XXXXXX";

    fd1 = mkstemp(template1);
    fd2 = mkstemp(template2);
    fd3 = mkstemp(template3);

    flags1 = fcntl(fd1, F_GETFD);
    if (flags1 == -1)
        errExit("fcntl - F_GETFD error");
    flags2 = fcntl(fd1, F_GETFD);
    if (flags2 == -1)
        errExit("fcntl - F_GETFD error");

    if (fcntl(fd1, F_SETFD, flags1 | FD_CLOEXEC) == -1)
        errExit("fcntl - F_SETFD error");
    if (fcntl(fd2, F_SETFD, flags2 | FD_CLOEXEC) == -1)
        errExit("fcntl - F_SETFD error");

    printf("Before exec:\n");
    print_fdlist(getpid());

    switch (pid = fork()) {
    case -1:
        errExit("fork error");
    case 0:
        execlp("sleep", "sleep", "60", NULL);
        errExit("execlp");
    default:
        sleep(1);
        printf("After exec:\n");
        print_fdlist(pid);
        if (kill(pid, SIGQUIT) == -1)
            errExit("kill error");
    }

    exit(EXIT_SUCCESS);
}
