#include <sys/mman.h>
#include <signal.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define TEST_FILE_PATH "/tmp/test"

static void sighandler(int sig) {
    char buf[128];
    snprintf(buf, 128, "Received Signal %d (%s)\n", sig, strsignal(sig));
    if (write(STDERR_FILENO, buf, strlen(buf)) == -1)
        errExit("write error");
}

int main(int argc, char const *argv[])
{
    int fd;
    long file_size, mem_size;
    char *buf;
    struct sigaction sa;

    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    printf("Page size: %ld\n", sysconf(_SC_PAGESIZE));

    file_size = sysconf(_SC_PAGESIZE);
    mem_size = file_size * 2;

    if (sigaction(SIGSEGV, &sa, NULL) == -1)
        errExit("sigaction setup error");
    if (sigaction(SIGBUS, &sa, NULL) == -1)
        errExit("sigaction setup error");

    fd = open(TEST_FILE_PATH, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("open test file error");
    if (ftruncate(fd, mem_size) == -1)
        errExit("expand test file error");

    buf = mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (buf == MAP_FAILED)
        errExit("mmap error");

    if (ftruncate(fd, file_size) == -1)
        errExit("shrink test file error");

    printf("Touch secure region\n");
    buf[file_size - 1] = '\0';
    printf("Touch out-of-file region\n");
    buf[file_size + 1] = '\0';
    printf("Touch out-of-memory region\n");
    buf[mem_size + 1] = '\0';

    if (munmap(buf, mem_size) == -1)
        errExit("mummap error");
    if (close(fd) == -1)
        errExit("close test file error");
    return 0;
}
