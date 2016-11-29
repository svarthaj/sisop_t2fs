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
        unsigned char **block = (unsigned char**)malloc(sb.blockSize*SECTOR_SIZE);
        fetch_inode(0, &inode, &sb);
        fetch_block(inode.dataPtr[0], block, &sb);

        printf("print from block\n");
        int i, j;
        for(i=0;i<sb.blockSize;i++){
            for(j=0;j<4;j++){
                BYTE *b = get_chunk((block+i), 0+(j*64), 0+(j*64));
                unsigned char *name = get_chunk((block+i), 1+(j*64), 31+(j*64));
                DWORD blocksSize = strtodword(get_chunk((block+i), 32+(j*64), 35+(j*64)));
                DWORD bytesSize = strtodword(get_chunk((block+i), 36+(j*64), 39+(j*64)));
                DWORD inode = strtodword(get_chunk((block+i), 40+(j*64), 43+(j*64))); 
                printf("dirent %d\ntypeval:%x\nname:%s\nblockSize:%d\nbytesSize:%d\ninode:%d\n", 4*i+j,*b, name, blocksSize, bytesSize, inode );
            }
        }
    }

    return 0;
}
