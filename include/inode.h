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
 * @name: string containing record name
 *
 * Searches in the inode for a directory entry with a specific name.
 *
 * Return: the offset of the record inside the inode or -1 if there is none.
 */
int inode_find_record(
	int index,
	char *name,
	struct t2fs_record *record,
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

#endif /* ifndef INODE_H */
