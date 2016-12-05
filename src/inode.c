#include <stdlib.h>
#include <string.h>
#include "logging.h"
#include "bitmap2.h"
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
	int next_block = bytes_to_int(index_block + offset%PTR_BYTE_SIZE);

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

	unsigned int ppb = sb->blockSize*SECTOR_SIZE/PTR_BYTE_SIZE;
	BYTE *index_block = alloc_buffer(sb->blockSize);

	fetch_block(pointer, index_block, sb);

	int next_pointer = bytes_to_int(index_block + (offset/ppb)*PTR_BYTE_SIZE);

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
	unsigned int ppb = sb->blockSize*SECTOR_SIZE/PTR_BYTE_SIZE;

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
	unsigned int len = umin(size_left, sb->blockSize*SECTOR_SIZE);
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
		len = umin(size_left, sb->blockSize*SECTOR_SIZE);
		memcpy(buffer + base, block + inf, len);

		bytes_read += len;
		base += bytes_read;
		size_left -= bytes_read;
	}

	free(block);

	return bytes_read;
}

/**
 * inode_find_record() - find a record in an inode
 * @index: inode index in the disk
 * @name: null terminated string containing record name
 * @record: pointer to a record to be filled with the entry
 * @sb: pointer to superblock structure
 *
 * Searches in the inode for a directory entry with a specific name. The string
 * `name` must be less than 31 characters long, not including the terminating
 * null character.
 *
 * Return: the offset of the record inside the inode or -1 if there is none.
 */
int inode_find_record(
	int index,
	char *name,
	struct t2fs_record *record,
	struct t2fs_superbloco *sb
){
	if (index < 0) {
		logwarning("inode_find_record: invalid inode index");
		return -1;
	}

	BYTE *bytes = (BYTE *)malloc(RECORD_BYTE_SIZE*sizeof(BYTE));

	if (!bytes) {
		logerror("inode_find_record: allocating memory");
		return -1;
	}

	unsigned int name_len = strlen(name);

	unsigned int offset = 0;
	int bytes_read = inode_read(index, offset, RECORD_BYTE_SIZE, bytes, sb);
	while (bytes_read == RECORD_BYTE_SIZE) {
		*record = bytes_to_record(bytes);
		char is_file = (record->TypeVal == 0x01);
		char is_dir = (record->TypeVal == 0x02);
		char match = (strncmp(record->name, name, name_len) == 0);

		if ((is_file || is_dir) && match) {
			free(bytes);
			return offset;
		} else {
			offset += RECORD_BYTE_SIZE;
		}

		bytes_read = inode_read(index, offset, RECORD_BYTE_SIZE, bytes, sb);
	}

	free(bytes);
	record->TypeVal = 0x00; /* no dirent found */
	return offset;
}

/**
 * new_inode() - allocate a new inode in disk
 *
 * Find a free inode, mark it as not free and return its index.
 *
 * Return: the index of the inode or -1 if it fails.
 */
int new_inode() {
	int i = searchBitmap2(BITMAP_INODE, 0);

	if (i == 0) {
		logwarning("new_inode: no more available inodes");
		return -1;
	}

	if (i < 0) {
		logerror("new_inode: finding inode");
		return -1;
	}

	setBitmap2(BITMAP_INODE, i, 1);
	return i;
}

/**
 * free_inode() - frees inode
 * @index: the index of the inode to be freed
 *
 * Mark the inode indexed by `index` as free
 *
 * Return: 0 if it suceeds, -1 otherwise.
 */
int free_inode(int index) {
	if (index < 0) {
		logwarning("free_inode: invalid index");
		return -1;
	}

	return setBitmap2(BITMAP_INODE, index, 0);
}
