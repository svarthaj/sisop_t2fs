#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "apidisk.h"
#include "bitmap2.h"
#include "t2fs.h"

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

WORD strtoword(unsigned char *str) {
    return *(str+1)*256+*(str);
}

DWORD strtodword(unsigned char *str) {
    return *(str+3)*16777216+*(str+2)*65536+*(str+1)*256+*(str);  
}

char* get_chunk(char *buffer, int start, int end) {
    char *str = (char*)malloc(sizeof(strlen(buffer)));
    
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
    
    return 0;   
}
