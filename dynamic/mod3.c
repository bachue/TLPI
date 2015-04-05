#include <stdio.h>
#include "mods.h"

int __attribute__ ((visibility("hidden"))) mod5(void);
void __attribute__ ((constructor)) load_mod3(void);
void __attribute__ ((destructor)) unload_mod3(void);

int mod5() {
    printf("mod5 from mod3\n");
    return 5;
}

void load_mod3() {
    printf("load mod3\n");
}

void unload_mod3() {
    printf("unload mod3\n");
}

int mod4(void);

int mod4() {
    printf("mod4 from mod3\n");
    return 4;
}

int mod3() {
    printf("\t%d\n", mod4());
    printf("mod3\n");
    return 3;
}
