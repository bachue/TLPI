#include <stdio.h>
#include "mods.h"

int mod5(void);

void __attribute__ ((constructor)) load_mod1(void);
void __attribute__ ((destructor)) unload_mod1(void);

void load_mod1() {
    printf("load mod1\n");
}

void unload_mod1() {
    printf("unload mod1\n");
}

int mod1() {
    printf("mod1\n");
    printf("mod5=%d\n", mod5());
    return 1;
}
