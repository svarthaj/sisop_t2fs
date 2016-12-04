#include <stdlib.h>
#include <string.h>
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
) {
	unsigned int dir = NUM_INODE_DIRECT_PTRS;
	unsigned int sind = NUM_INODE_SINGLE_IND_PTRS;
	unsigned int dind = NUM_INODE_DOUBLE_IND_PTRS;
	unsigned int ppb = sb->blockSize*SECTOR_SIZE/PTR_SIZE;

	if (offset < dir) {
		if (inode->dataPtr[offset] != INVALID_PTR) {
			return inode->dataPtr[offset];
		} else {
			return -2;
		}
	} else if (offset < dir + sind*ppb) {
		return inode_follow_once(offset - dir, inode->singleIndPtr, sb);
	} else if (offset < dir + sind*ppb + dind*ppb*ppb) {
		return inode_follow_twice(offset - dir - sind*ppb - dind*ppb*ppb, inode->doubleIndPtr, sb);
	} else {
		logwarning("inode_get_block_number: tried to access beyond inode");
		return -3;
	}
}

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
) {
	struct t2fs_inode inode;

	if (fetch_inode(index, &inode, sb) != 0) {
		logwarning("read_inode: fetching inode");
		return -1;
	}

	unsigned int block_byte_size = (sb->blockSize*SECTOR_SIZE);
	unsigned int block_offset = offset/block_byte_size;
	unsigned int bytes_read = 0;
	unsigned int size_left = size;
	int block_number = inode_get_block_number(&inode, block_offset, sb);

	BYTE *block = alloc_buffer(sb->blockSize);
	if (block_number < 0 || fetch_block(block_number, block, sb) != 0) {
		loginfo("inode_read: couldn't fetch a block");
		flogdebug("inode_read: inode %d, block offset %u, block number: %d", index, block_offset, block_number);
		free(block);
		return bytes_read;
	}

	unsigned int base = 0;
	unsigned int inf = offset%block_byte_size;
	unsigned int len = umin(size_left, sb->blockSize);
	memcpy(buffer + base, block + inf, len);

	bytes_read += len;
	base += bytes_read;
	size_left -= bytes_read;

	while (size_left > 0) {
		block_offset += 1;
		block_number = inode_get_block_number(&inode, block_offset, sb);
		if (block_number < 0 || fetch_block(block_number, block, sb) != 0) {
			loginfo("inode_read: couldn't fetch a block");
			flogdebug("inode_read: inode %d, block offset %u", index, block_offset);
			free(block);
			return bytes_read;
		}

		inf = 0;
		len = umin(size_left, sb->blockSize);
		memcpy(buffer + base, block + inf, len);

		bytes_read += len;
		base += bytes_read;
		size_left -= bytes_read;
	}

	free(block);

	return bytes_read;
}