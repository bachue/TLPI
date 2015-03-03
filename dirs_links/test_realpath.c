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
static void create_path_node(struct path_node**, struct path_node**);
static void free_path_nodes(struct path_node*);

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

{
    struct path_node *cur = head;
    while (cur != NULL) {
        printf("%s (%ld) -> ", cur->path, (long) strlen(cur->path));
        cur = cur->next;
    }
    printf("EOF \n");
}

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
    struct path_node *head = *phead, *tail = *ptail, *tmp;

    if (path[i] == '/') { // First char is /, absolute path
        free_path_nodes(head);
        head = tail = NULL;
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
                if (tail != NULL) {
                    tmp = tail;
                    tail = tail->prev;
                    free(tmp);
                    if (tail != NULL)
                        tail->next = NULL;
                    else
                        head = NULL;
                }
                i += 2;
                break;
            }
            // For others, fallback to default segment
        default:
            for (j = i; path[j] != '/' && path[j] != '\0'; j++)
                ;
            if (j - i > NAME_MAX) fatal("Name exceeds limits");
            create_path_node(&head, &tail);
            strncpy(tail->path, path + i, j - i);
            tail->path[j - i] = '\0';
            i = j;
        }
    }
    *phead = head;
    *ptail = tail;
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

void free_path_nodes(struct path_node *head) {
    struct path_node *cur = head;
    while (head != NULL) {
        cur = head;
        head = head->next;
        free(cur);
    }
}
