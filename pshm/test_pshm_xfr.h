#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define SEM_READER_PATH "/xfr.reader"
#define SEM_WRITER_PATH "/xfr.writer"
#define SHM_PATH "/xfr"

#define BUF_SIZE (1024 * 8)

struct shmseg {                 /* Defines structure of shared memory segment */
    int cnt;                    /* Number of bytes used in 'buf' */
    char buf[BUF_SIZE];         /* Data being transferred */
};
