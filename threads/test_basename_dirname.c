#include "tlpi_hdr.h"
#include <pthread.h>
#include <limits.h>

static __thread char buf[PATH_MAX + 1];

char* basename_s(char *);
char* dirname_s(char *);

static void * print_basename(void *arg)
{
    printf("basename: %s\n", basename_s((char *) arg));
    return NULL;
}

static void * print_dirname(void *arg)
{
    printf("dirname: %s\n", dirname_s((char *) arg));
    return NULL;
}

int main(int argc, char  *argv[]) {
    pthread_t t1, t2, t3, t4;
    int s;

    s = pthread_create(&t1, NULL, print_basename, (void *) "/tmp/a/b/c/d");
    if (s != 0)
        errExitEN(s, "pthread_create - 1");

    s = pthread_create(&t2, NULL, print_dirname, (void *) "/tmp/a/b/c/d");
    if (s != 0)
        errExitEN(s, "pthread_create - 2");

    s = pthread_create(&t3, NULL, print_basename, (void *) "/tmp");
    if (s != 0)
        errExitEN(s, "pthread_create - 3");

    s = pthread_create(&t4, NULL, print_dirname, (void *) "/tmp");
    if (s != 0)
        errExitEN(s, "pthread_create - 4");

    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join - 1");

    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join - 2");

    s = pthread_join(t3, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join - 3");

    s = pthread_join(t4, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join - 4");

    exit(EXIT_SUCCESS);

    return 0;
}

char* basename_s(char *path) {
    char *ptr;
    ptr = rindex(path, '/') + 1;
    strncpy(buf, ptr, PATH_MAX);
    buf[PATH_MAX] = '\0';
    return buf;
}

char* dirname_s(char *path) {
    char *ptr;
    size_t size;
    ptr = rindex(path, '/');
    size = ptr - path;
    if (size == 0) {
        size = 1;
        strncpy(buf, "/", 1);
    } else
        strncpy(buf, path, size);
    buf[size] = '\0';
    return buf;
}
