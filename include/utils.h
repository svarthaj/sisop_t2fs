#ifndef UTILS_H
#define UTILS_H

#include "t2fs.h"

/**
 * print_inode() - print inode structure to stderr
 * @sb: pointer to the inode structure
 *
 * Print the fields of a inode structure to the stderr.
 *
 * Return:
 */
void print_inode(struct t2fs_inode *inode);

/**
 * print_superblock() - print superblock structure to stderr
 * @sb: pointer to the superblock structure
 *
 * Print the fields of a superblock structure to the stderr.
 *
 * Return:
 */
void print_superblock(struct t2fs_superbloco *sb);

/**
 * print_record() - print record structure to stderr
 * @record: pointer to record struct
 *
 * Prints the fields of a record
 *
 * Return:
 */
void print_record(struct t2fs_record *record);

/**
 * umax() - return maximum between two numbers
 * @a: a number
 * @b: a number
 *
 * Return: the maximum of a and b. They must be unsigned integers.
 */
unsigned int umax(unsigned int a, unsigned int b);

/**
 * umin() - return minimum between two numbers
 * @a: a number
 * @b: a number
 *
 * Return: the minimum of a and b. They must be unsigned integers.
 */
unsigned int umin(unsigned int a, unsigned int b);

/**
 * alloc_buffer() - allocate buffer
 * @size: size of the buffer in sectors
 *
 * Return a pointer to a buffer of size `size`*SECTOR_SIZE.
 *
 * Return: the pointer to the allocated area.
 */
BYTE *alloc_buffer(int size);

/**
 * reverse_endian() - reverse the endianess of a buffer
 * @buffer: pointer to the buffer
 * @len: length of the buffer
 *
 * Turns little endian buffer into a big endian buffer.
 *
 * Return:
 */
void reverse_endianess(BYTE *buffer, int len);

/**
 * bytes_to_word() - return word represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the word represented by a little endian buffer of two bytes.
 *
 * Return: a WORD
 */
WORD bytes_to_word(BYTE *buffer);

/**
 * bytes_to_dword() - return dword represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the dword represented by a little endian buffer of two bytes.
 *
 * Return: a DWORD
 */
DWORD bytes_to_dword(BYTE *buffer);

/**
 * bytes_to_int() - return int represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the int represented by a little endian buffer of two bytes.
 *
 * Return: an integer
 */
int bytes_to_int(BYTE *buffer);

/**
 * bytes_to_record() - return record represented by bytes array
 * @buffer: pointer to the buffer containing the record data
 *
 * Return the record structure represented by the bytes array taken directly
 * from disk. The bytes array must be at least 64 bytes long.
 *
 * Return: a record structure.
 */
struct t2fs_record bytes_to_record(BYTE *buffer);

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
 * the file's record.
 *
 * If the parent directory exists, but the file does not, `record` will have
 * a TypeVal of 0x00, and `ip` will be the parent directory's inode number.
 * `offset` could be anything in this case.
 *
 * If the parent directory does not exist (and consequently the file does not
 * exist too), `record` will have a TypeVal of 0x00, and `ip` will be -1.
 * `offset` could be anything in this case.
 *
 * Return: 0 if succeeds, -1 otherwise.
 */
//int find_record(
//	struct
//) {
//
//}


/**
 * sane_path() - return if path is sane
 * @path: pointer to string representing path
 *
 * Checks if all names in path are less than or equal to 31 characters in
 * length, if it has a initial "/" and if it does not have a trailing "/"
 *
 * Return: 0 if path is sane, -1 if it's not.
 */
int sane_path(char *path);

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
struct list* split_path(char *path);

#endif /* ifndef UTILS_H */
