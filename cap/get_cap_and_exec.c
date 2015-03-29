#undef _POSIX_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/capability.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

void print_cap(char*, uint32_t, uint32_t, uint32_t);
void inherit_all_permitted(uint32_t);

void print_cap(char *prompt, uint32_t permitted, uint32_t effective, uint32_t inheritable) {
    int i;
    cap_t set = cap_init();
    size_t length = sizeof(uint32_t) * 8;
    for (i = 0; i < length; i++) {
        if (permitted & (1 << i))
            if (cap_set_flag(set, CAP_PERMITTED, 1, &i, CAP_SET) == -1) {
                perror("cap_set_flag error");
                exit(EXIT_FAILURE);
            }
    }
    for (i = 0; i < length; i++) {
        if (effective & (1 << i))
            if (cap_set_flag(set, CAP_EFFECTIVE, 1, &i, CAP_SET) == -1) {
                perror("cap_set_flag error");
                exit(EXIT_FAILURE);
            }
    }
    for (i = 0; i < length; i++) {
        if (inheritable & (1 << i))
            if (cap_set_flag(set, CAP_INHERITABLE, 1, &i, CAP_SET) == -1) {
                perror("cap_set_flag error");
                exit(EXIT_FAILURE);
            }
    }
    printf("%s %s\n", prompt, cap_to_text(set, NULL));
    if (cap_free(set) == -1) {
        perror("cap_set_flag error");
        exit(EXIT_FAILURE);
    }
}

void inherit_all_permitted(uint32_t permitted) {
    int i;
    size_t length = sizeof(uint32_t) * 8;
    cap_t set = cap_get_proc();
    for (i = 0; i < length; i++) {
        if (permitted & (i << i))
            printf("Inherit %s from permitted\n", cap_to_name(i));
            if (cap_set_flag(set, CAP_INHERITABLE, 1, &i, CAP_SET) == -1) {
                perror("cap_set_flag error");
                exit(EXIT_FAILURE);
            }
    }
    if (cap_set_proc(set) == -1) {
        perror("cap_set_proc error");
        exit(EXIT_FAILURE);
    }
    if (cap_free(set) == -1) {
        perror("cap_set_flag error");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    struct __user_cap_header_struct cap_header_data;
    cap_user_header_t cap_header = &cap_header_data;

    struct __user_cap_data_struct cap_data_data;
    cap_user_data_t cap_data = &cap_data_data;

    char *program = NULL;

    if (argc > 2 || (argc > 1 && strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s [program]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argc == 2)
        program = argv[1];

    cap_header->pid = getpid();
    cap_header->version = _LINUX_CAPABILITY_VERSION_1;

    if (capget(cap_header, cap_data) < 0) {
        perror("failed to capget");
        exit(EXIT_FAILURE);
    }

    print_cap(argv[0], cap_data->permitted, cap_data->effective, cap_data->inheritable);

    inherit_all_permitted(cap_data->permitted);

    if (capget(cap_header, cap_data) < 0) {
        perror("failed to capget");
        exit(EXIT_FAILURE);
    }
    print_cap(argv[0], cap_data->permitted, cap_data->effective, cap_data->inheritable);

    if (program != NULL) {
        execlp(program, program, NULL);
        perror("Failed to exec");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
