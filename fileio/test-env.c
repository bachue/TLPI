#include "tlpi_hdr.h"

#define ENV "TEST=abc"

void test1(void);
void test2(void);
void test3(void);
void test4(void);
void test5(void);
void test6(void);
void test7(void);

int main(int argc, char  *argv[]) {
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s case\n", argv[0]);
    }

    if (strcmp(argv[1], "1") == 0)
        test1();
    else if (strcmp(argv[1], "2") == 0)
        test2();
    else if (strcmp(argv[1], "3") == 0)
        test3();
    else if (strcmp(argv[1], "4") == 0)
        test4();
    else if (strcmp(argv[1], "5") == 0)
        test5();
    else if (strcmp(argv[1], "6") == 0)
        test6();
    else if (strcmp(argv[1], "7") == 0)
        test7();
    else
        usageErr("%s case\n", argv[0]);

    execlp("printenv", "printenv");
    errExit("exec error");
    exit(EXIT_FAILURE);
}

void test1() {
    putenv(ENV);
}

void test2() {
    char env[] = ENV;
    putenv(env);
}

void test3() {
    char *env = (char *) malloc(strlen(ENV) + 1);
    strcpy(env, ENV);
    putenv(env);
    strcpy(env + 5, "def");
}

void test4() {
    setenv("TEST", "abc", 1);
}

void test5() {
    char name[] = "TEST", value[] = "abc";
    setenv(name, value, 1);
}

void test6() {
    char *name = (char *) malloc(strlen("TEST") + 1);
    char *value = (char *) malloc(strlen("abc") + 1);
    strcpy(name, "TEST");
    strcpy(value, "abc");
    setenv(name, value, 1);
    strcpy(value, "def");
}

void test7() {
    clearenv();
}
