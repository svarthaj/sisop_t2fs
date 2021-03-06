#include <stdlib.h>
#include <string.h>
#include "inode.h"
#include "list.h"
#include "logging.h"
#include "utils.h"

void print_superblock (struct t2fs_superbloco *sb) {
    flogdebug("id: %c%c%c%c", sb->id[0], sb->id[1], sb->id[2], sb->id[3]);
    flogdebug("version: %x", sb->version);
    flogdebug("superBlockSize: %hu", sb->superblockSize);
    flogdebug("freeBlocksBitmapSize: %hu", sb->freeBlocksBitmapSize);
    flogdebug("freeInodeBitmapSize: %hu", sb->freeInodeBitmapSize);
    flogdebug("inodeAreaSize: %hu", sb->inodeAreaSize);
    flogdebug("blockSize: %hu", sb->blockSize);
    flogdebug("diskSize: %u", sb->diskSize);
}

void print_inode (struct t2fs_inode *inode) {
    flogdebug("dataPtr[0]: %d", inode->dataPtr[0]);
    flogdebug("dataPtr[1]: %d", inode->dataPtr[1]);
    flogdebug("singleIndPtr: %d", inode->singleIndPtr);
    flogdebug("doubleIndPtr: %d", inode->doubleIndPtr);
}

/**
 * print_record() - print record structure to stderr
 * @record: pointer to record struct
 *
 * Prints the fields of a record
 *
 * Return:
 */
void print_record(struct t2fs_record *record) {
	char name[32];
	strncpy(name, record->name, 31);
	name[31] = '\0';

    flogdebug("TypeVal: %d", record->TypeVal);
    flogdebug("name: %s", name);
    flogdebug("blocksFileSize: %u", record->blocksFileSize);
    flogdebug("bytesFileSize: %u", record->bytesFileSize);
    flogdebug("inodeNumber: %d", record->inodeNumber);
}

/**
 * umax() - return maximum between two numbers
 * @a: a number
 * @b: a number
 *
 * Return: the maximum of a and b. They must be unsigned integers.
 */
unsigned int umax(unsigned int a, unsigned int b) {
	return a < b ? b : a;
}

/**
 * umin() - return minimum between two numbers
 * @a: a number
 * @b: a number
 *
 * Return: the minimum of a and b. They must be unsigned integers.
 */
unsigned int umin(unsigned int a, unsigned int b) {
	return a > b ? b : a;
}

/**
 * alloc_buffer() - allocate buffer
 * @size: size of the buffer in sectors
 *
 * Return a pointer to a buffer of size `size`*SECTOR_SIZE.
 *
 * Return: the pointer to the allocated area.
 */
BYTE *alloc_buffer(int size) {
	BYTE *buffer = (BYTE *)malloc(size*SECTOR_SIZE*sizeof(BYTE));

	if (!buffer) {
		logerror("alloc_buffer: allocating buffer");
		exit(1);
	}

	return buffer;
}

/**
 * reverse_endian() - reverse the endianess of a buffer
 * @buffer: pointer to the buffer
 * @len: length of the buffer
 *
 * Turns little endian buffer into a big endian buffer.
 *
 * Return:
 */
void reverse_endianess(BYTE *buffer, int len) {
	BYTE b;
	int i, j;

	for (i = 0, j = len - 1; i < j; i++, j--)
	{
		b = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = b;
	}
}

/**
 * bytes_to_word() - return word represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the word represented by a little endian buffer of two bytes.
 *
 * Return: a WORD
 */
WORD bytes_to_word(BYTE *buffer) {
	BYTE tmp[] = {0, 0};
	memcpy(tmp, buffer, 2);
	reverse_endianess(tmp, 2);

	return ((WORD)tmp[0]<<8) + (WORD)tmp[1];
}

int word_to_bytes(WORD w, BYTE *buffer) {
	BYTE tmp[2];

    for(int j = 0; j < 2; j++)
	    tmp[j] = (w>>(8-j*8)) & 0xFF;

	reverse_endianess(tmp, 2);
	memcpy(buffer, tmp, 2);

	return 0;
}

/**
 * bytes_to_dword() - return dword represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the dword represented by a little endian buffer of two bytes.
 *
 * Return: a DWORD
 */
DWORD bytes_to_dword(BYTE *buffer) {
	BYTE tmp[] = {0, 0, 0, 0};
	memcpy(tmp, buffer, 4);
	reverse_endianess(tmp, 4);

	return ((DWORD)tmp[0]<<24) + ((DWORD)tmp[1]<<16) + ((DWORD)tmp[2]<<8) + (DWORD)tmp[3];
}

int dword_to_bytes(DWORD dw, BYTE *buffer) {
	BYTE tmp[4];

    for(int j = 0; j < 4; j++)
	    tmp[j] = (dw>>(24-j*8)) & 0xFF;

	reverse_endianess(tmp, 4);
	memcpy(buffer, tmp, 4);

	return 0;
}

/**
 * bytes_to_int() - return int represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the int represented by a little endian buffer of two bytes.
 *
 * Return: an integer
 */
int bytes_to_int(BYTE *buffer) {
	BYTE tmp[] = {0, 0, 0, 0};
	memcpy(tmp, buffer, 4);
	reverse_endianess(tmp, 4);

	return (int)(tmp[0]<<24) + (int)(tmp[1]<<16) + (int)(tmp[2]<<8) + (int)tmp[3];
}

int int_to_bytes(int i, BYTE *buffer) {
	BYTE tmp[4];

    for(int j = 0; j < 4; j++)
	    tmp[j] = (i>>(24-j*8)) & 0xFF;

	reverse_endianess(tmp, 4);
	memcpy(buffer, tmp, 4);

	return 0;
}

/**
 * bytes_to_record() - return record represented by bytes array
 * @buffer: pointer to the buffer containing the record data
 *
 * Return the record structure represented by the bytes array taken directly
 * from disk. The bytes array must be at least 64 bytes long.
 *
 * Return: a record structure.
 */
struct t2fs_record bytes_to_record(BYTE *buffer) {
	struct t2fs_record record;

	record.TypeVal = buffer[0];
	memcpy(record.name, buffer + 1, 31);
	record.blocksFileSize = bytes_to_dword(buffer + 32);
	record.bytesFileSize = bytes_to_dword(buffer + 36);
	record.inodeNumber = bytes_to_int(buffer + 40);

	return record;
}

int record_to_bytes(struct t2fs_record record, BYTE *buffer){
    buffer[0] = record.TypeVal;
    memcpy(buffer + 1, record.name, 31);
    dword_to_bytes(record.blocksFileSize, buffer + 32);
    dword_to_bytes(record.bytesFileSize, buffer + 36);
    int_to_bytes(record.inodeNumber, buffer + 40);

    return 0;
}

/**
 * sane_path() - return if path is sane
 * @path: pointer to string representing path
 *
 * Checks if all names in path are less than or equal to 31 characters in
 * length, if it has a initial "/" and if it does not have a trailing "/"
 *
 * Return: 0 if path is sane, -1 if it's not.
 */
int sane_path(char *path)
{
	int len = strlen(path);

	if (len == 1 && path[len - 1] == '/') {
		return 0;
	}

	if (path[len - 1] == '/') {
		logwarning("sane_path: path must not have trailing slash");
		return -1;
	}

	if (path[0] != '/') {
		logwarning("sane_path: path must begin with '/'");
		return -1;
	}

	int name_sz = 0;
	for (int i = 1; i < len; i++) {
		if (path[i] == '/' && name_sz == 0) {
			logwarning("sane_path: path must not contain empty names");
			return -1;
		}

		if (path[i] == '/' && name_sz > 0) {
			name_sz = 0;
		} else {
			name_sz += 1;
		}

		if (name_sz > 31) {
			logwarning("sane_path: path musta have names under 31 characters");
			return -1;
		}
	}

	return 0;
}

/**
 * split_path() - short description
 * @path: non-constant string representing the path
 *
 * Returns a list representing the path given by `path`. `path` *will* be
 * modified by strtok()! Also `path`'s memory area must live as long as the
 * list returned lives. In other words, only free(path) if destroy_list()
 * is called on the list right after.
 *
 * Return: a list structure
 */
struct list *split_path(char *path) {
	if (sane_path(path) != 0) {
		return NULL;
	}

    struct list *path_list = create_list();

    char *tok;
	tok = strtok(path, "/");
	while(tok != NULL){
		append_list(path_list, tok);
		tok = strtok(NULL, "/");
	}
	first_list(path_list);
	return path_list;
}

/**
 * find_record() - find the record of a file
 * @names: list with the names forming the path to the file
 * @record: pointer to a record to be filled by the function
 * @ip: pointer to an index to be filled by the function, it represents
 * the parent inode index
 * @offset: pointer to an offset to be filled by the function
 * @sb: pointer to the superblock structure
 *
 * Finds the record of the file with path represented by the list of names
 * `names`. Pythonly, '/' + '/'.join(names) would be the string representing
 * the path to the file.
 *
 * If the file exists and is found, `ip` will be the
 * index of the parent directory's inode, `offset` will be the offset of the
 * record inside the parent directory's inode, and `record` will be filled with
 * the file's record. The function returns 0, in this case.
 *
 * If the parent directory exists, but the file does not, `record` will have
 * a TypeVal of 0x00, and `ip` will be the parent directory's inode number.
 * `offset` could be anything in this case, and the function returns 0.
 *
 * If the parent directory does not exist (and consequently the file does not
 * exist too), `record` will have a TypeVal of 0x00, and `ip` will be -1.
 * `offset` could be anything in this case. The function returns 0 in this case.
 *
 * Return: 0 if succeeds, -1 otherwise.
 */
int find_record(
	struct list *names,
	struct t2fs_record *record,
	int *ip,
	unsigned int *offset,
	struct t2fs_superbloco *sb
) {
	*ip = 0; // start on root

	first_list(names);

	inode_find_record(*ip, getnode_list(names), offset, record, sb);
	next_list(names);

	while (names->it != NULL) {
		if (record->TypeVal == 0x00) {
			*ip = -1;
			*offset = 0;
			return 0;
		}

		*ip = record->inodeNumber;
		inode_find_record(*ip, getnode_list(names), offset, record, sb);
		next_list(names);
	}

	return 0;
}
