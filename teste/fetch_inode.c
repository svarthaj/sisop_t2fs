#include <stdlib.h>
#include <stdio.h>
#include "bitmap2.h"
#include "t2fs.h"

int main(int argc, char **argv) {
    struct t2fs_superbloco sb;
    struct t2fs_inode inode;
    struct t2fs_inode inode1;

    if ( fetch_superblock(&sb) != 0)
        return -1;
    else {
        fetch_inode(0, &inode, &sb);
        print_inode(&inode);
    }

    return 0;
}
