#include <stdio.h>
#include "mods.h"

int mod5(int);

int mod5(int x) {
    printf("mod5 x = %d\n", x);
    return x;
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
