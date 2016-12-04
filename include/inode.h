#ifndef INODE_H
#define INODE_H

#include "t2fs.h"

#define NUM_INODE_DIRECT_PTRS 2
#define NUM_INODE_SINGLE_IND_PTRS 1
#define NUM_INODE_DOUBLE_IND_PTRS 1

/**
 * inode_follow_once() - derefer a pointer in an inode
 * @offset: offset *inside single indirect tree*
 * @pointer: single indirect index block number
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
 * @offset: offset *inside double indirect tree*
 * @pointer: double indirect index block number
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
 *
 * Return the number of a data block on the disk given the inode and the
 * logical block offset inside the inode.
 *
 * Return: a block number or -2 if there is an invalid pointer on the way, and
 * -3 if offset is out of limits.
 */
int inode_get_block_number(
	struct t2fs_inode *inode,
	unsigned int block_offset,
	struct t2fs_superbloco *sb
);

#endif /* ifndef INODE_H */
