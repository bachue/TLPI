#define _GNU_SOURCE             /* Get remap_file_pages() declaration
                                   from <sys/mman.h> */
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int fd, j;
    char ch;
    long pageSize;
    char *src_addr, *dst_addr;

    fd = open("/tmp/tfile", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("open");

    pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize == -1)
        fatal("Couldn't determine page size");

    for (ch = 'a'; ch < 'd'; ch++)
        for (j = 0; j < pageSize; j++)
            write(fd, &ch, 1);

    system("od -a /tmp/tfile");

    src_addr = mmap(0, 3 * pageSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (src_addr == MAP_FAILED)
        errExit("mmap");

    printf("Mapped at address %p\n", src_addr);

    dst_addr = mmap(0, 3 * pageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (dst_addr == MAP_FAILED)
        errExit("mmap");

    /* The three pages of the file -- 0 1 2 -- are currently mapped
       linearly. Now we rearrange the mapping to 2 1 0. */

    if (mmap(dst_addr, pageSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 2 * pageSize) == MAP_FAILED)
        errExit("mmap");
    if (mmap(dst_addr + 2 * pageSize, pageSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0) == MAP_FAILED)
        errExit("mmap");

    /* Now we modify the contents of the mapping */

    for (j = 0; j < 0x100; j++)         /* Modifies page 2 of file */
        *(dst_addr + j) = '0';
    for (j = 0; j < 0x100; j++)         /* Modifies page 0 of file */
        *(dst_addr + 2 * pageSize + j) = '2';

    system("od -a /tmp/tfile");

    exit(EXIT_SUCCESS);
}
