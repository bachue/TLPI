#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include "tlpi_hdr.h"

struct path_node {
    struct path_node *next;
    struct path_node *prev;
    char path[NAME_MAX + 1];
};

static void generate_path_nodes(struct path_node**, struct path_node**, char*);
static void generate_path_nodes_via_cwd(struct path_node**, struct path_node**);
static void dereference_path_nodes(struct path_node**, struct path_node**);
static void create_path_node(struct path_node**, struct path_node**);
static void shift_path_node(struct path_node**, struct path_node**);
static void free_path_nodes(struct path_node*);
static void generate_path(char*, size_t, struct path_node*);
static void print_path(struct path_node*);

int main(int argc, char  *argv[]) {
    char *path;
    struct path_node *head = NULL, *tail = NULL;
    if (argc != 2 || (argc > 1 && strcmp(argv[1], "--help") == 0))
        usageErr("%s file\n", argv[0]);
    path = argv[1];

    if (path[0] == '/')
        head = NULL;
    else
        generate_path_nodes_via_cwd(&head, &tail);

    generate_path_nodes(&head, &tail, path);

    print_path(head);

    free_path_nodes(head);
    exit(EXIT_SUCCESS);
}

void generate_path_nodes_via_cwd(struct path_node **phead, struct path_node **ptail) {
    char absolutepath[PATH_MAX + 1];
    if (getcwd(absolutepath, PATH_MAX) == NULL)
        errExit("Failed to get current directory");
    absolutepath[PATH_MAX] = '\0';
    generate_path_nodes(phead, ptail, absolutepath);
}

void generate_path_nodes(struct path_node **phead, struct path_node **ptail, char *path) {
    int i = 0, j = 0;

    if (path[i] == '/') { // First char is /, absolute path
        free_path_nodes(*phead);
        *phead = *ptail = NULL;
        for (i++; path[i] == '/'; i++)
            ;
    }

    while (path[i] != '\0') {
        switch (path[i]) {
        case '/':
            for (i++; path[i] == '/'; i++)
                ;
            break;
        case '.':
            if (path[i + 1] == '/' || path[i + 1] == '\0') {
                i++;
                break;
            } else if (path[i + 1] == '.' && (path[i + 2] == '/' || path[i + 2] == '\0')) {
                shift_path_node(phead, ptail);
                i += 2;
                break;
            }
            // For others, fallback to default segment
        default:
            for (j = i; path[j] != '/' && path[j] != '\0'; j++)
                ;
            if (j - i > NAME_MAX) fatal("Name exceeds limits\n");
            create_path_node(phead, ptail);
            strncpy((*ptail)->path, path + i, j - i);
            (*ptail)->path[j - i] = '\0';
            i = j;
            dereference_path_nodes(phead, ptail);
        }
    }
}

void dereference_path_nodes(struct path_node **phead, struct path_node **ptail) {
    char path_buf[PATH_MAX + 1], def_buf[PATH_MAX + 1];
    struct stat st;
    ssize_t size;
    generate_path(path_buf, PATH_MAX, *phead);
    path_buf[PATH_MAX] = '\0';
    if (lstat(path_buf, &st) == -1)
        errExit("lstat error");
    if (S_ISLNK(st.st_mode)) {
        size = readlink(path_buf, def_buf, PATH_MAX);
        if (size == -1)
            errExit("readlink error");
        def_buf[size] = '\0';
        shift_path_node(phead, ptail);
        generate_path_nodes(phead, ptail, def_buf);
    }
}

void create_path_node(struct path_node **phead, struct path_node **ptail) {
    struct path_node *tmp = (struct path_node*) malloc(sizeof(struct path_node));
    if (*phead != NULL) {
        (*ptail)->next = tmp;
        tmp->prev = *ptail;
        *ptail = tmp;
    } else {
        *phead = *ptail = tmp;
        (*phead)->prev = NULL;
    }
}

void shift_path_node(struct path_node **phead, struct path_node **ptail) {
    struct path_node *tmp;
    if (*ptail != NULL) {
        tmp = *ptail;
        *ptail = (*ptail)->prev;
        free(tmp);
        if (*ptail != NULL)
            (*ptail)->next = NULL;
        else
            *phead = NULL;
    }
}

void free_path_nodes(struct path_node *head) {
    struct path_node *cur = head;
    while (head != NULL) {
        cur = head;
        head = head->next;
        free(cur);
    }
}

void generate_path(char *buf, size_t size, struct path_node *head) {
    struct path_node *cur = head;
    buf[0] = '\0';
    while (cur != NULL) {
        strncat(buf, "/", size);
        strncat(buf, cur->path, size);
        cur = cur->next;
    }
}

void print_path(struct path_node *head) {
    char buf[PATH_MAX + 1];
    generate_path(buf, PATH_MAX, head);
    buf[PATH_MAX] = '\0';
    printf("%s\n", buf);
}
