#include <sys/inotify.h>
#include <limits.h>
#include <ftw.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

static void             /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

static int inotifyFd;
static char *wds;

static void add_watch(const char *pathname) {
    int wd;
    wd = inotify_add_watch(inotifyFd, pathname, IN_ALL_EVENTS);
    if (wd == -1)
        errExit("inotify_add_watch");

    strncpy(wds + wd * (PATH_MAX+1), pathname, PATH_MAX);
    *(wds + (wd + 1) * (PATH_MAX+1)) = '\0';
    printf("Watching %s using wd %d\n", pathname, wd);
}

static int watch_on(const char *pathname, const struct stat *sbuf, int flag, struct FTW *ftwb) {
    if (S_ISDIR(sbuf->st_mode))
        add_watch(pathname);
    return 0;
}

static void watch_tree(const char *path) {
    if (nftw(path, watch_on, 10, FTW_PHYS) == -1)
        errExit("nftw error");
}

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int
main(int argc, char *argv[])
{
    int j, fd;
    long max;
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    char path[PATH_MAX + 1];
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname...\n", argv[0]);

    inotifyFd = inotify_init();                 /* Create inotify instance */
    if (inotifyFd == -1)
        errExit("inotify_init");

    fd = open("/proc/sys/fs/inotify/max_user_watches", O_RDONLY);
    if (fd == -1)
        errExit("failed to open /proc/sys/fs/inotify/max_user_watches");
    if (read(fd, buf, BUF_LEN) <= 0)
        errExit("failed to read /proc/sys/fs/inotify/max_user_watches");
    max = strtol(buf, NULL, 10);
    if (max <= 0)
        fatal("invalid max_user_watches");
    wds = (char *) malloc(max * (PATH_MAX + 1));
    if (wds == NULL)
        fatal("failed to allocate memory");

    /* For each command-line argument, add a watch for all events */

    for (j = 1; j < argc; j++)
        watch_tree(argv[j]);

    for (;;) {                                  /* Read events forever */
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0)
            fatal("read() from inotify fd returned 0!");

        if (numRead == -1)
            errExit("read");

        printf("Read %ld bytes from inotify fd\n", (long) numRead);

        /* Process all of the events in buffer returned by read() */

        for (p = buf; p < buf + numRead; ) {
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);

            if (event->mask & IN_CREATE && event->mask & IN_ISDIR) {
                strncpy(path, wds + event->wd * (PATH_MAX + 1), PATH_MAX);
                strncat(path, "/", PATH_MAX);
                strncat(path, event->name, PATH_MAX);
                path[PATH_MAX] = '\0';
                add_watch(path);
            }

            p += sizeof(struct inotify_event) + event->len;
        }
    }

    free(wds);
    exit(EXIT_SUCCESS);
}
