#include <stdio.h>
#include "mods.h"

int mod4(void);

int mod4() {
    printf("mod4 from main\n");
    return 4;
}

int main(int argc, char const *argv[])
{
    printf("\t%d\n", mod1());
    printf("\t%d\n", mod2());
    printf("\t%d\n", mod3());
    printf("\t%d\n", mod4());
    return 0;
}
