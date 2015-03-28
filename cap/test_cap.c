#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#undef _POSIX_SOURCE
#include <sys/capability.h>
#include <sys/wait.h>

extern int errno;

void whoami(void);
void listCaps(char*);

void whoami()
{
    printf("uid=%i euid=%i gid=%i\n", getuid(), geteuid(), getgid());
}

void listCaps(char *prompt)
{
    cap_t caps = cap_get_proc();
    ssize_t y = 0;
    printf("%s: The process %d was give capabilities %s\n", prompt, (int) getpid(), cap_to_text(caps, &y));
    fflush(0);
    cap_free(caps);
}

int main(int argc, char **argv)
{
    whoami();
    pid_t parentPid = getpid();

    if(!parentPid)
        return 1;
    cap_t caps = cap_init();

    cap_value_t capList[5] = {
        CAP_NET_RAW, CAP_NET_BIND_SERVICE, CAP_SETUID, CAP_SETGID, CAP_SETPCAP
    };

    unsigned num_caps = 5;
    cap_set_flag(caps, CAP_EFFECTIVE, num_caps, capList, CAP_SET);
    cap_set_flag(caps, CAP_INHERITABLE, num_caps, capList, CAP_SET);
    cap_set_flag(caps, CAP_PERMITTED, num_caps, capList, CAP_SET);

    if (cap_set_proc(caps)) {
        perror("capset()");
        return EXIT_FAILURE;
    }
    listCaps("Parent: before drop caps");

    switch (fork()) {
    case -1:
        perror("fork()");
        return EXIT_FAILURE;
    case 0:
        listCaps("Child");
        _exit(EXIT_SUCCESS);
    }

    printf("dropping caps\n");
    cap_clear(caps);  // resetting caps storage

    if (cap_set_proc(caps)) {
        perror("capset()");
        return EXIT_FAILURE;
    }
    listCaps("Parent: after drop caps");
    cap_free(caps);

    wait(NULL);
    exit(EXIT_SUCCESS);
}
