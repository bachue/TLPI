#define _GNU_SOURCE
#include <utmpx.h>
#include <paths.h>              /* Definitions of _PATH_UTMP and _PATH_WTMP */
#include "tlpi_hdr.h"

static char* test_getlogin(void);
static char user[256];

char* test_getlogin() {
    struct utmpx *ut;
    char *tty = ttyname(STDIN_FILENO);
    setutxent();
    while ((ut = getutxent()) != NULL) {
        if (strcmp(ut->ut_line, tty + 5) == 0) {
            strncpy(user, ut->ut_user, 255);
            user[255] = '\0';
            break;
        }
    }
    endutxent();
    return user;
}

int main(int argc, char const *argv[])
{
    printf("getlogin(): %s\n", test_getlogin());
    return 0;
}
