#include <pwd.h>
#include "tlpi_hdr.h"

struct passwd* getpwnam(const char *login) {
    struct passwd *result = NULL, *pwd;
    while ((pwd = getpwent()) != NULL) {
        if (strcmp(pwd->pw_name, login) == 0) {
            result = pwd;
            break;
        }
    }
    endpwent();
    return result;
}

int
main(int argc, char *argv[])
{
    struct passwd *pwd = getpwnam("root");
    printf("%s %s %ld %ld\n", pwd->pw_name, pwd->pw_passwd,
                              (long) pwd->pw_uid, (long) pwd->pw_gid);
    exit(EXIT_SUCCESS);
}
