#include "tlpi_hdr.h"
#include <pwd.h>
#include <unistd.h>
#include <termios.h>

#define _PASSWORD_LEN 128
static char buf[_PASSWORD_LEN];
char* mygetpass(const char *);

char* mygetpass(const char *prompt) {
    struct termios tp, save;
    int i;

    if (tcgetattr(STDIN_FILENO, &tp) == -1)
        errExit("tcgetattr error");
    save = tp;                          /* So we can restore settings later */
    tp.c_lflag &= ~ECHO;                /* ECHO off, other bits unchanged */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1)
        errExit("tcsetattr error");

    fflush(stdout);
    if (write(STDOUT_FILENO, prompt, strlen(prompt)) == -1)
        errExit("output error");

    if (fgets(buf, _PASSWORD_LEN, stdin) == NULL)
        fatal("Got end-of-file/error on fgets()\n");

    for (i = 0; buf[i] != '\n' || buf[i] == '\0'; ++i)
        ;
    buf[i] = '\0';

    if (tcsetattr(STDIN_FILENO, TCSANOW, &save) == -1)
        errExit("tcsetattr");

    if (write(STDOUT_FILENO, "\n", 1) == -1)
        errExit("output error");

    return buf;
}

int main(int argc, char *argv[]) {
    printf("%s\n", mygetpass("Password: "));
    return 0;
}
