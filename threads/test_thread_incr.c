#include <pthread.h>
#include "tlpi_hdr.h"

struct thr_t {
    pthread_t *thread_ptr;
    int loops;
};

static volatile int glob = 0;   /* "volatile" prevents compiler optimizations
                                   of arithmetic operations on 'glob' */
static void *                   /* Loop 'arg' times incrementing 'glob' */
threadFunc(void *arg)
{
    int loops = ((struct thr_t *) arg)->loops;
    int loc, j;
    pthread_t *thread_ptr = ((struct thr_t *) arg)->thread_ptr;

    for (j = 0; j < loops; j++) {
        loc = glob;
        loc++;
        glob = loc;
        printf("thread: %ld, glob: %d\n", (long) *thread_ptr, glob);
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int loops, s;
    struct thr_t th1, th2;

    loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;

    th1.thread_ptr = &t1;
    th1.loops = loops;

    s = pthread_create(&t1, NULL, threadFunc, (void *) &th1);
    if (s != 0)
        errExitEN(s, "pthread_create");

    th2.thread_ptr = &t2;
    th2.loops = loops;

    s = pthread_create(&t2, NULL, threadFunc, (void *) &th2);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}
