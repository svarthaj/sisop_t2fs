#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "apidisk.h"
#include "bitmap2.h"
#include "t2fs.h"

/*------------------------------------------------------------------------------
Funções auxiliares
------------------------------------------------------------------------------*/

void print_superblock (struct t2fs_superbloco *sb) {
    printf("id:%s\n", sb->id);
    printf("version:%x\n", sb->version);   
    printf("superBlockSize:%hu\n", sb->superblockSize);   
    printf("freeBlocksBitmapSize:%hu\n", sb->freeBlocksBitmapSize);   
    printf("freeInodeBitmapSize:%hu\n", sb->freeInodeBitmapSize);   
    printf("indodeAreaSize:%hu\n", sb->inodeAreaSize);   
    printf("blockSize:%hu\n", sb->blockSize);   
    printf("diskSize:%u\n", sb->diskSize);
}

void print_inode (struct t2fs_inode *inode) {
    printf("dataPtr[0]:%d\n", inode->dataPtr[0]);
    printf("dataPtr[1]:%d\n", inode->dataPtr[1]);   
    printf("singleIndPtr:%d\n", inode->singleIndPtr);   
    printf("doubleIndPtr:%d\n", inode->doubleIndPtr);   
}

WORD strtoword(unsigned char *str) {
    return *(str+1)*256+*(str);
}

DWORD strtodword(unsigned char *str) {
    return *(str+3)*16777216+*(str+2)*65536+*(str+1)*256+*(str);  
}

int strtoint(unsigned char *str) {
    return *(str+3)*16777216+*(str+2)*65536+*(str+1)*256+*(str);  
}

char* get_chunk(char *buffer, int start, int end) {
    char *str = (char*)malloc(end-start+1);
    
    int i;
    for (i=start; i<=end; i++) {
        *(str+(i-start)) = *(buffer+i);
    }

    return str;
}

int fetch_superblock (struct t2fs_superbloco *sb) {
    unsigned char *buffer = (unsigned char*)malloc(SECTOR_SIZE);
    unsigned char *dword = (unsigned char*)malloc(4);
    unsigned char *word = (unsigned char*)malloc(2);

    if ( read_sector(0, buffer) != 0) {
        puts("error reading superblock");
        puts("closing application");
        return -1;
    }

    dword = get_chunk(buffer, 0, 3);
    strncpy(sb->id, dword, 4);

    word = get_chunk(buffer, 4, 5);
    sb->version = strtoword(word); 
    
    word = get_chunk(buffer, 6, 7);
    sb->superblockSize = strtoword(word); 
     
    word = get_chunk(buffer, 8, 9);
    sb->freeBlocksBitmapSize = strtoword(word); 
     
    word = get_chunk(buffer, 10, 11);
    sb->freeInodeBitmapSize = strtoword(word); 
     
    word = get_chunk(buffer, 12, 13);
    sb->inodeAreaSize = strtoword(word); 
     
    word = get_chunk(buffer, 14, 15);
    sb->blockSize = strtoword(word); 
     
    dword = get_chunk(buffer, 16, 19);
    sb->diskSize = strtodword(dword);
 
    free(buffer);
    free(word);
    free(dword);

    return 0;   
}

int fetch_inode(int inode_number, struct t2fs_inode *inode, struct t2fs_superbloco *sb) {
    unsigned char *buffer = (unsigned char*)malloc(SECTOR_SIZE);
    unsigned char *integer = (unsigned char*)malloc(4);

    int inode_sector = inode_number / 16; //16 inodes per sector
    int sector_number = sb->superblockSize + sb->freeBlocksBitmapSize + sb->freeInodeBitmapSize + inode_sector;

    if ( read_sector(sector_number, buffer) != 0) {
        puts("error reading sector\n");
        puts("closing application\n");
        return -1;
    }

    integer = get_chunk(buffer, 0, 3);
    inode->dataPtr[0] = strtoint(integer);

    integer = get_chunk(buffer, 4, 7);
    inode->dataPtr[1] = strtoint(integer);
    
    integer = get_chunk(buffer, 8, 11);
    inode->singleIndPtr = strtoint(integer);
    
    integer = get_chunk(buffer, 12, 15);
    inode->doubleIndPtr = strtoint(integer);

    free(buffer);
    free(integer);

    return 0;
}

int fetch_block(int block_number, unsigned char **block, struct t2fs_superbloco *sb) {
    unsigned char *buffer = (unsigned char*)malloc(SECTOR_SIZE);

    int block_sector = block_number * 16; //16 blocks per sector
    int first_sector = sb->superblockSize + sb->freeBlocksBitmapSize + sb->freeInodeBitmapSize + sb->inodeAreaSize + block_sector;
    int last_sector = first_sector + sb->blockSize - 1;

    printf("print from buffer\n"); 
    int i, j;
    for (i=first_sector, j=0; i<=last_sector; i++, j++) {
        if ( read_sector(i, buffer) != 0) {
            puts("error reading datablock\n");
            puts("closing application\n");
            return -1;
        }
        int k;
        for(k=0;k<4;k++){ 
            BYTE *b = get_chunk(buffer, 0+(k*64), 0+(k*64));
            BYTE *name = get_chunk(buffer, 1+(k*64), 31+(k*64));
            DWORD blocksSize = strtodword(get_chunk(buffer, 32+(k*64), 35+(k*64)));
            DWORD bytesSize = strtodword(get_chunk(buffer, 36+(k*64), 39+(k*64)));
            DWORD inode = strtodword(get_chunk(buffer, 40+(k*64), 43+(k*64))); 
           
            printf("dirent %d\ntypeval:%x\nname:%s\nblockSize:%d\nbytesSize:%d\ninode:%d\n", 4*j+k, *b, name, blocksSize, bytesSize, inode );
        }
        memcpy(block+j,buffer,SECTOR_SIZE);
    }
    printf("print from block\n");
    BYTE *b = get_chunk(block, 0, 0);
    BYTE *name = get_chunk(block, 1, 31);
    DWORD blocksSize = strtodword(get_chunk(block, 32, 35));
    DWORD bytesSize = strtodword(get_chunk(block, 36, 39));
    DWORD inode = strtodword(get_chunk(block, 40, 43)); 
   
    printf("dirent %d\ntypeval:%x\nname:%s\nblockSize:%d\nbytesSize:%d\ninode:%d\n", j, *b, name, blocksSize, bytesSize, inode );
    return 0;
}

/*------------------------------------------------------------------------------
Funções obrigatorias
------------------------------------------------------------------------------*/
int identify2 (char *name, int size) {
    char *id = "Lucas Leal (206438)\nLuis Mollmann (206440)";
    strncpy(name, id, size);

    if (strncmp(name, id, size) == 0)
        return 0;
    
    return -1;
}

FILE2 create2 (char *filename) {
    return -1;
}

int delete2 (char *filname) {
    return -1;
}

FILE2 open2 (char *filname) {
    return -1;
}

int close2 (FILE2 handle) {
    return -1;
}

int read2 (FILE2 handle, char *buffer, int size) {
    return -1;
}

int write2 (FILE2 handle, char *buffer, int size) {
    return -1;
}

int truncate2 (FILE2 handle) {
    return -1;
}

int seek2 (FILE2 handle, DWORD offset) {
    return -1;
}

int mkdir2 (char *pathname) {
    return -1;
}

int rmdir2 (char *pathname) {
    return -1;
}

DIR2 opendir (char *pathname) {
    return -1;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry) {
    return -1;
}

int closedir2 (DIR2 handle) {
    return -1;
}

