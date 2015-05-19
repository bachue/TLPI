#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include "inet_sockets.h"       /* Declares our socket functions */
#include "tlpi_hdr.h"

#define ADDRSTR_LEN 4096
#define BUFFER_SIZE 4096

static struct keyValPairList {
    char key[BUFFER_SIZE];
    char value[BUFFER_SIZE];
    struct keyValPairList *next, *prev;
};

inline static struct keyValPairList* search_key(struct keyValPairList *head, char *key) {
    struct keyValPairList *ptr;
    for (ptr = head; ptr != NULL; ptr = ptr->next) {
        if (strncmp(ptr->key, key, BUFFER_SIZE) == 0)
            return ptr;
    }
    return NULL;
}

inline static void insert_key_value(struct keyValPairList **phead, char *key, char *value) {
    struct keyValPairList *ptr, *new;
    ptr = search_key(*phead, key);
    if (ptr == NULL) {
        if (*phead != NULL) {
            for (ptr = *phead; ptr->next != NULL; ptr = ptr->next)
                ;
        }
        new = (struct keyValPairList *) malloc(sizeof(struct keyValPairList));
        strncpy(new->key, key, BUFFER_SIZE);
        new->key[BUFFER_SIZE - 1] = '\0';
        strncpy(new->value, value, BUFFER_SIZE);
        new->value[BUFFER_SIZE - 1] = '\0';
        new->prev = ptr;
        new->next = NULL;
        if (*phead != NULL)
            ptr->next = new;
        else
            *phead = new;
    } else {
        strncpy(ptr->value, value, BUFFER_SIZE);
        ptr->value[BUFFER_SIZE - 1] = '\0';
    }
}

inline static void delete_key(struct keyValPairList **phead, char *key) {
    struct keyValPairList *ptr;
    ptr = search_key(*phead, key);
    if (ptr == NULL) return;
    if (ptr->next != NULL)
        ptr->next->prev = ptr->prev;
    if (ptr->prev != NULL)
        ptr->prev->next = ptr->next;
    else // then ptr is the first node;
        *phead = ptr->next;
    free(ptr);
}

int main(int argc, char const *argv[])
{
    int lfd, cfd;
    struct sockaddr *claddr;
    socklen_t addrlen, alen;
    char addrStr[ADDRSTR_LEN], buffer[BUFFER_SIZE], *cmd, *key, *val;
    struct keyValPairList *head = NULL, *ptr;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errExit("ignore SIGPIPE error");
    lfd = inetListen("5000", 5, &addrlen);
    if (lfd == -1)
        errExit("inetListen error");
    claddr = (struct sockaddr *) malloc(addrlen);
    if (claddr == NULL)
        errExit("malloc error");
    for (;;) {
        alen = addrlen;
        cfd = accept(lfd, claddr, &alen);
        if (cfd == -1) {
            errMsg("accept error");
            continue;
        }
        printf("Connection from %s\n", inetAddressStr(claddr, alen, addrStr, ADDRSTR_LEN));

        memset(buffer, 0, BUFFER_SIZE);
        if (read(cfd, buffer, BUFFER_SIZE) == -1)
            errExit("read error");
        buffer[BUFFER_SIZE - 1] = '\0';
        cmd = buffer;
        key = index(cmd, ' ');
        if (key != NULL) {
            *key++ = '\0';
            val = index(key, ' ');
            if (val != NULL) *val++ = '\0';
        }

        if (strncmp(cmd, "get", BUFFER_SIZE) == 0) {
            ptr = search_key(head, key);
            if (ptr != NULL) {
                if (write(cfd, ptr->value, strlen(ptr->value)) != strlen(ptr->value))
                    errExit("write error");
            }
        } else if (strncmp(cmd, "set", BUFFER_SIZE) == 0) {
            insert_key_value(&head, key, val);
        } else if (strncmp(cmd, "del", BUFFER_SIZE) == 0) {
            delete_key(&head, key);
        } else if (strncmp(cmd, "list", BUFFER_SIZE) == 0) {
            for (ptr = head; ptr != NULL; ptr = ptr->next) {
                snprintf(buffer, BUFFER_SIZE - 1, "%s = %s\n", ptr->key, ptr->value);
                buffer[BUFFER_SIZE - 1] = '\0';
                if (write(cfd, buffer, strlen(buffer)) != strlen(buffer))
                    errExit("write error");
            }
        }
        if (close(cfd) == -1)
            errExit("close error");
    }
    return 0;
}
