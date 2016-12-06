#ifndef INODE_H
#define INODE_H

#include "t2fs.h"

#define NUM_INODE_DIRECT_PTRS 2
#define NUM_INODE_SINGLE_IND_PTRS 1
#define NUM_INODE_DOUBLE_IND_PTRS 1

/**
 * inode_follow_once() - derefer a pointer in an inode
 * @offset: block offset *inside single indirect tree*
 * @pointer: single indirect index block number
 * @sb: pointer to superblock structure
 *
 * Returns the pointer for the block containing the data at the offset `offset`
 * inside the single indirect tree pointed by `pointer`.
 *
 * Return: a block number or -2 if there is an invalid pointer on the way.
 */
int inode_follow_once(
	unsigned int offset,
	int pointer,
	struct t2fs_superbloco *sb
);

/**
 * inode_follow_twice() - derefer a pointer twice in an inode
 * @offset: block offset *inside double indirect tree*
 * @pointer: double indirect index block number
 * @sb: pointer to superblock structure
 *
 * Returns the pointer for the block containing the data at the offset `offset`
 * inside the double indirect tree pointed by `pointer`.
 *
 * Return: a block number or -2 if there is an invalid pointer on the way.
 */
int inode_follow_twice(
	unsigned int offset,
	int pointer,
	struct t2fs_superbloco *sb
);

/**
 * inode_get_block_number() - return block number from inode offset
 * @inode: inode struct
 * @offset: block offset inside inode
 * @sb: pointer to superblock structure
 *
 * Return the number of a data block on the disk given the inode and the
 * logical block offset inside the inode.
 *
 * Return: a block number or -2 if there is an invalid pointer on the way, and
 * -3 if offset is out of limits.
 */
int inode_get_block_number(
	struct t2fs_inode *inode,
	unsigned int offset,
	struct t2fs_superbloco *sb
);

/**
 * inode_read() - read inode data
 * @index: inode index in the disk
 * @offset: byte offset inside inode
 * @size: size of read
 * @buffer: pointer to the buffer that will store the read data
 *
 * Read inode data in inode indexed by `index` from byte offset `offset` to
 * `offset + size`.
 *
 * Return: number of bytes that could be read, -1 in case of failure.
 */
int inode_read(
	int index,
	unsigned int offset,
	unsigned int size,
	BYTE *buffer,
	struct t2fs_superbloco *sb
);

/**
 * inode_find_record() - find a record in an inode
 * @index: inode index in the disk
 * @name: null terminated string containing record name
 * @record: pointer to a record to be filled with the entry
 * @sb: pointer to superblock structure
 * @offset: pointer to an unsigned int to be filled with the offset of the
 * record, if found.
 *
 * Searches in the inode for a directory entry with a specific name. The string
 * `name` must be less than 31 characters long, not including the terminating
 * null character.
 *
 * Return: 0 if succeeds or -1 if there is no such record.
 */
int inode_find_record(
	int index,
	char *name,
	unsigned int *offset,
	struct t2fs_record *record,
	struct t2fs_superbloco *sb
);

/**
 * inode_find_free_record() - find a free record in an inode
 * @index: inode index in the disk
 * @offset: pointer to an unsigned int to be filled with the offset of the
 * record, if found.
 * @sb: pointer to superblock structure
 *
 * Searches in the inode for a free directory entry and returns its offset.
 *
 * Return: 0 if succeeds, -1 otherwise.
 */
int inode_find_free_record(
	int index,
	unsigned int *offset,
	struct t2fs_superbloco *sb
);

/**
 * new_inode() - allocate a new inode in disk
 *
 * Find a free inode, mark it as not free and return its index.
 *
 * Return: the index of the inode or -1 if it fails.
 */
int new_inode();

/**
 * free_inode() - frees inode
 * @index: the index of the inode to be freed
 *
 * Mark the inode indexed by `index` as free
 *
 * Return: 0 if it suceeds, -1 otherwise.
 */
int free_inode(int index);

/**
 * inode_write() - write inode data
 * @index: inode index in the disk
 * @offset: byte offset inside inode
 * @size: size of write
 * @buffer: pointer to the buffer that stores the data to be written
 *
 * Write inode data in inode indexed by `index` from byte offset `offset` to
 * `offset + size`.
 *
 * Return: number of bytes that could be writen, -1 in case of failure.
 */
int inode_write(
	int index,
	unsigned int offset,
	unsigned int size,
	BYTE *buffer,
	struct t2fs_superbloco *sb
);

int inode_add_block(
	int index,
	struct t2fs_inode *inode,
	unsigned int offset,
	struct t2fs_superbloco *sb
);

/**
 * inide_add_block_direct() - add a block to direct pointers
 * @inode: inode struct
 * @offset: block offset inside inode
 * @sb: pointer to superblock structure
 *
 * Return: 0 if succeeds, -1 if there are no more blocks in the disk, -2 if
 * there is an invalid pointer o the way, -3 if offset is off limits.
 */
int inode_add_block_direct(
	int index,
	struct t2fs_inode *inode,
	unsigned int offset,
	struct t2fs_superbloco *sb
);

int inode_add_block_single_ind(
	int index,
	struct t2fs_inode *inode,
	unsigned int offset,
	struct t2fs_superbloco *sb
);

int inode_add_block_double_ind(
	int index,
	struct t2fs_inode *inode,
	unsigned int offset,
	struct t2fs_superbloco *sb
);

/* update inode in disk. returns 0 if successful and -1 otherwise. */
int update_inode(
	int index,
	struct t2fs_inode *inode,
	struct t2fs_superbloco *sb
);

int add_pointer_to_index_block(
	int index_block_number,
	int data_block_number,
	unsigned int offset,
	struct t2fs_superbloco *sb
);

#endif /* ifndef INODE_H */
