#define _GNU_SOURCE
#include <pwd.h>
#include <shadow.h>
#include <grp.h>
#include "tlpi_hdr.h"

void usage(char*);

void usage(char *program) {
    usageErr("%s: [-u user] program-file arg1 arg2 ...\n", program);
}

int main(int argc, char  *argv[]) {
    char *cuser, *tuser, *password, *encrypted;
    int optidx;
    struct passwd *pwd;
    struct spwd *spwd;
    if (argc == 1) usage(argv[0]);
    if (strcmp(argv[1], "-u") == 0) {
        if (argc > 2) {
            tuser = argv[2];
            optidx = 3;
        } else {
            usage(argv[0]);
        }
    } else {
        tuser = "root";
        optidx = 1;
    }

    errno = 0;
    pwd = getpwuid(getuid());
    if (pwd == NULL) {
        if (errno)
            errExit("getpwnam error");
        else
            fatal("couldn't find user in /etc/passwd");
    }

    errno = 0;
    cuser = pwd->pw_name;
    spwd = getspnam(cuser);
    if (spwd == NULL && errno)
        errExit("getspnam error");
    if (spwd != NULL)
        pwd->pw_passwd = spwd->sp_pwdp;

    password = getpass("Password: ");
    encrypted = crypt(password, pwd->pw_passwd);
    memset(password, 0, strlen(password));

    if (encrypted == NULL)
        errExit("crypt error");

    if (strcmp(encrypted, pwd->pw_passwd) != 0)
        fatal("Password unmatch");

    errno = 0;
    pwd = getpwnam(tuser);
    if (pwd == NULL) {
        if (errno)
            errExit("getpwnam error");
        else
            fatal("couldn't find effective user in /etc/passwd");
    }

    if (setresgid(pwd->pw_gid, pwd->pw_gid, pwd->pw_gid) == -1)
        errExit("setresgid error");
    if (setgroups(1, &pwd->pw_gid) == -1)
        errExit("setgroups error");
    if (setresuid(pwd->pw_uid, pwd->pw_uid, pwd->pw_uid) == -1)
        errExit("setresuid error");
    execvp(argv[optidx], &argv[optidx]);
    errExit("execvp error");
}
