#include <stdlib.h>
#include "logging.h"
#include "apidisk.h"
#include "t2fs.h"
#include "disk.h"
#include "utils.h"
#include "inode.h"

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
) {
	if (pointer == INVALID_PTR) {
		logwarning("inode_follow_once: invalid pointer");
		return -2;
	}

	BYTE *index_block = alloc_buffer(sb->blockSize);

	fetch_block(pointer, index_block, sb);
	int next_block = bytes_to_int(index_block + offset%PTR_SIZE);

	free(index_block);

	return next_block;
}

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
) {
	if (pointer == INVALID_PTR) {
		logwarning("inode_follow_twice: invalid pointer");
		return -2;
	}

	unsigned int ppb = sb->blockSize*SECTOR_SIZE/PTR_SIZE;
	BYTE *index_block = alloc_buffer(sb->blockSize);

	fetch_block(pointer, index_block, sb);

	int next_pointer = bytes_to_int(index_block + (offset/ppb)*PTR_SIZE);

	free(index_block);

	return inode_follow_once(offset%ppb, next_pointer, sb);
}

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
) {
	unsigned int dir = NUM_INODE_DIRECT_PTRS;
	unsigned int sind = NUM_INODE_SINGLE_IND_PTRS;
	unsigned int dind = NUM_INODE_DOUBLE_IND_PTRS;
	unsigned int ppb = sb->blockSize*SECTOR_SIZE/PTR_SIZE;

	if (block_offset < dir) {
		if (inode->dataPtr[block_offset] != INVALID_PTR) {
			return inode->dataPtr[block_offset];
		} else {
			return -2;
		}
	} else if (block_offset < dir + sind*ppb) {
		return inode_follow_once(block_offset - dir, inode->singleIndPtr, sb);
	} else if (block_offset < dir + sind*ppb + dind*ppb*ppb) {
		return inode_follow_twice(block_offset - dir - sind*ppb - dind*ppb*ppb, inode->doubleIndPtr, sb);
	} else {
		logwarning("inode_get_block_number: tried to access beyond inode");
		return -3;
	}
}
