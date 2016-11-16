#include <stdlib.h>
#include <stdio.h>
#include "t2fs.h"

int main(int argc, char **argv) {
    struct t2fs_superbloco sb;

    if ( fetch_superblock(&sb) != 0)
        return -1;

    else {
        print_superblock(&sb);
    }

    return 0;
}
