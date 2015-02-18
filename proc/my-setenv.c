#include "tlpi_hdr.h"

extern char **environ;

int my_setenv(const char *name, const char *value, int overwrite);
int my_unsetenv(const char *name);

int main(int argc, char  *argv[]) {
    char **ep;
    printf("1: %d\n", my_setenv("TEST", "abc", 0));
    printf("2: %d\n", my_setenv("TEST", "def", 0));

    for (ep = environ; *ep != NULL; ep++) {
        puts(*ep);
    }

    puts("---------");

    printf("3: %d\n", my_unsetenv("TEST"));
    printf("4: %d\n", my_unsetenv("TEST"));

    for (ep = environ; *ep != NULL; ep++) {
        puts(*ep);
    }

    exit(EXIT_SUCCESS);
}

int my_setenv(const char *name, const char *value, int overwrite) {
    size_t size;
    char *ptr;
    if (!overwrite) {
        if (getenv(name) != NULL)
            return 0;
    }
    size = strlen(name) + strlen(value) + 2;
    ptr = (char *) malloc(size);
    strcat(ptr, name);
    strcat(ptr, "=");
    strcat(ptr, value);
    return putenv(ptr);
}

int my_unsetenv(const char *name) {
    return putenv((char *) name);
}
