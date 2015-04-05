#include <stdio.h>
#include "mods.h"

void __attribute__ ((constructor)) load_mod2(void);
void __attribute__ ((destructor)) unload_mod2(void);

void load_mod2() {
    printf("load mod2\n");
}

void unload_mod2() {
    printf("unload mod2\n");
}

int mod2() {
    printf("mod2\n");
    return 2;
}
