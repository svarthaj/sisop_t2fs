#include <string.h>
#include <stdlib.h>
#include "logging.h"
#include "open_file.h"
#include "disk.h"
#include "utils.h"
#include "list.h"
#include "t2fs.h"
#include "inode.h"

int identify2 (char *name, int size) {
    char *id = "Lucas Leal (206438)\nLuis Mollmann (206440)";

    strncpy(name, id, size);

    if (strncmp(name, id, size) != 0) {
		logwarning("identify2: couldn't copy correctly");
		return -1;
	}

    return 0;
}

int generic_open(char *filename, struct list *names, int type){
    struct t2fs_record record;
    int ip;
    unsigned int oe;
    struct t2fs_superbloco sb;

    if(fetch_superblock(&sb) != 0){
        logerror("generic open: superblock fetch");
		return -1;
    }

    if(find_record(names, &record, &ip, &oe, &sb) != 0){
		return -1;
    }

	if (record.TypeVal != type) {
		return -1;
	}

    FILE2 handle = create_open_file(filename, &record, ip, oe);

    return handle;
}

FILE2 open2 (char *filename) {
	int len = strlen(filename);
	char *buffer = (char *)malloc((len + 1)*sizeof(char));
	strcpy(buffer, filename);

    struct list *names = split_path(buffer);

    if(names == NULL){
		logerror("open2: split_path returned NULL");
		free(buffer);
		return -1;
    }

	FILE2 handle = (FILE2)generic_open(filename, names, 0x01);
	free(buffer);

	return handle;
}

int generic_create(struct list *names, int type) {
    struct t2fs_record record;
	int ip;
	unsigned int oe;
	struct t2fs_superbloco sb;
	if (fetch_superblock(&sb) == -1) return -1;

    find_record(names, &record, &ip, &oe, &sb);
	if (record.TypeVal != 0x00 && ip >= 0) {
		logwarning("generic_create: file exists");
		return -1;
	}

	if (ip == -1) {
		logwarning("generic_create: path does not exist");
	}

	int i = new_inode();

	if (i == -1) {
		logwarning("generic_create: no more free inodes");
		return -1;
	}

	record.TypeVal = type;
	last_list(names);
	strcpy(record.name, getnode_list(names));
	record.blocksFileSize = 0;
	record.bytesFileSize = 0;
	record.inodeNumber = i;

	if (inode_find_free_record(ip, &oe, &sb) != 0) {
		logwarning("no free directory entry");
		return -1;
	}

	BYTE *buffer = (BYTE *)malloc(RECORD_BYTE_SIZE*sizeof(BYTE));
	record_to_bytes(record, buffer);
	inode_write(ip, oe, RECORD_BYTE_SIZE, buffer, &sb);
	free(buffer);

	return 0;
}

FILE2 create2 (char *filename){
	int len = strlen(filename);
	char *buffer = (char *)malloc((len + 1)*sizeof(char));
	strcpy(buffer, filename);

    struct list *names = split_path(buffer);

    if(names == NULL){
		logerror("open2: split_path returned NULL");
		free(buffer);
		return -1;
    }

	if (generic_create(names, 0x01) < 0) {
		return -1;
	}
	free(buffer);

	return open2(filename);
}

FILE2 delete (char *filename){
    return -1;
}

int generic_close(int handle){
    return destroy_open_file(handle);
}

int close2 (FILE2 handle){
    return generic_close((int)handle);
}

int read2 (FILE2 handle, char *buffer, int size){
    struct open_file *this_file;
    int i;
    int of;
    int bytes_read;

	get_open_file(handle, &this_file);

    i = this_file->full_record.inodeNumber;
    of = this_file->file_offset;
	struct t2fs_superbloco sb;
	if (fetch_superblock(&sb) == -1) return -1;
	print_record(&this_file->full_record);
    bytes_read = inode_read(i, of, size, (BYTE *)buffer, &sb); // must check parameters
    this_file->file_offset += bytes_read;

    return bytes_read;
}

int write2 (FILE2 handle, char *buffer, int size){
    return -1;
}

int truncate2 (FILE2 handle){
    return -1;
}

int seek2 (FILE2 handle, DWORD offset){
    return -1;
}

int mkdir2 (char *pathname){
	int len = strlen(pathname);
	char *buffer = (char *)malloc((len + 1)*sizeof(char));
	strcpy(buffer, pathname);

    struct list *names = split_path(buffer);

    if(names == NULL){
		logerror("open2: split_path returned NULL");
		free(buffer);
		return -1;
    }

	if (generic_create(names, 0x02) < 0) {
		return -1;
	}
	free(buffer);

	return opendir2(pathname);
}

int rmdir2 (char *pathname){
    return -1;
}

DIR2 opendir2 (char *pathname){
	int len = strlen(pathname);
	char *buffer = (char *)malloc((len + 1)*sizeof(char));
	strcpy(buffer, pathname);

    struct list *names = split_path(buffer);

    if(names == NULL){
		logerror("opendir2: split_path returned NULL");
		return -1;
    }

	DIR2 handle = (DIR2)generic_open(pathname, names, 0x02);

	return handle;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry){
    return -1;
}

int closedir2 (DIR2 handle){
    return generic_close((int)handle);
}
