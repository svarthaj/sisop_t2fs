#include <stdlib.h>
#include <stdio.h>
#include "t2fs.h"

int main(int argc, char **argv) {
    char name[256];

    if( identify2(name, 256) == 0 )
        puts(name);
    else puts("ERROR");

    return 0;
    
}
