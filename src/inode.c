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
	int next_block = bytes_to_int(index_block + offset*PTR_BYTE_SIZE);

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

	if (index_block == NULL) {
		logdebug("null");
	}

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
		return inode_follow_twice(offset - dir - sind*ppb, inode->doubleIndPtr, sb);
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
		logwarning("inode_read: couldn't fetch inode");
		return -1;
	}

	unsigned int block_byte_size = (sb->blockSize*SECTOR_SIZE);
	unsigned int block_offset = offset/block_byte_size;
	unsigned int bytes_read = 0;
	unsigned int size_left = size;
	int block_number = inode_get_block_number(&inode, block_offset, sb);

	BYTE *block = alloc_buffer(sb->blockSize);
	if (block_number < 0) {
		logdebug("inode_read: end of inode");
		free(block);
		return bytes_read;
	}

	if (fetch_block(block_number, block, sb) != 0) {
		loginfo("inode_read: couldn't fetch a block");
		flogdebug("inode_read: inode %d, block offset %u", index, block_offset);
		free(block);
		return bytes_read;
	}

	unsigned int base = 0;
	unsigned int inf = offset%block_byte_size;
	unsigned int len = umin(size_left, sb->blockSize*SECTOR_SIZE);
	memcpy(buffer + base, block + inf, len);

	bytes_read += len;
	base += bytes_read;
	size_left -= len;

	while (size_left > 0) {
		block_offset += 1;
		block_number = inode_get_block_number(&inode, block_offset, sb);

		if (block_number < 0) {
			logdebug("inode_read: end of inode");
			free(block);
			return bytes_read;
		}

		if (fetch_block(block_number, block, sb) != 0) {
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
		size_left -= len;
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
){
	BYTE *bytes = (BYTE *)malloc(RECORD_BYTE_SIZE*sizeof(BYTE));

	if (!bytes) {
		logerror("inode_find_record: allocating memory");
		return -1;
	}

	unsigned int name_len = strlen(name);

	*offset = 0;
	int bytes_read = inode_read(index, *offset, RECORD_BYTE_SIZE, bytes, sb);
	if (bytes_read == -1) return -1;

	while (bytes_read == RECORD_BYTE_SIZE) {
		*record = bytes_to_record(bytes);
		char is_file = (record->TypeVal == 0x01);
		char is_dir = (record->TypeVal == 0x02);
		char match = (strncmp(record->name, name, name_len) == 0);

		if ((is_file || is_dir) && match) {
			free(bytes);
			return 0;
		} else {
			*offset += RECORD_BYTE_SIZE;
		}

		bytes_read = inode_read(index, *offset, RECORD_BYTE_SIZE, bytes, sb);
		if (bytes_read == -1) return -1;
	}

	free(bytes);
	record->TypeVal = 0x00; /* no dirent found */
	return -1;
}

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
) {
	if (index < 0) {
		logwarning("inode_find_record: invalid inode index");
		return -1;
	}

	BYTE *bytes = (BYTE *)malloc(RECORD_BYTE_SIZE*sizeof(BYTE));

	if (!bytes) {
		logerror("inode_find_record: allocating memory");
		return -1;
	}

	*offset = 0;
	int bytes_read = inode_read(index, *offset, RECORD_BYTE_SIZE, bytes, sb);
	while (bytes_read == RECORD_BYTE_SIZE) {
		struct t2fs_record record = bytes_to_record(bytes);
		char is_file = (record.TypeVal == 0x01);
		char is_dir = (record.TypeVal == 0x02);

		if (!(is_file || is_dir)) {
			free(bytes);
			return 0;
		} else {
			*offset += RECORD_BYTE_SIZE;
		}

		bytes_read = inode_read(index, *offset, RECORD_BYTE_SIZE, bytes, sb);
	}

	free(bytes);
	return -1;
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
) {
	struct t2fs_inode inode;

	if (fetch_inode(index, &inode, sb) != 0) {
		logwarning("inode_read: couldn't fetch inode");
		return -1;
	}

	unsigned int block_byte_size = (sb->blockSize*SECTOR_SIZE);
	unsigned int block_offset = offset/block_byte_size;
	unsigned int bytes_written = 0;
	unsigned int size_left = size;
	int block_number = inode_get_block_number(&inode, block_offset, sb);

	BYTE *block = alloc_buffer(sb->blockSize);
	if (block_number == -2) {
		logdebug("inode_write: allocating more space");
		inode_add_block(index, &inode, block_offset, sb);
		block_number = inode_get_block_number(&inode, block_offset, sb);
	}

	if (fetch_block(block_number, block, sb) != 0) {
		loginfo("inode_write: couldn't fetch a block");
		flogdebug("inode_write: inode %d, block offset %u", index, block_offset);
		free(block);
		return bytes_written;
	}

	unsigned int base = 0;
	unsigned int inf = offset%block_byte_size;
	unsigned int len = umin(size_left, sb->blockSize*SECTOR_SIZE);
	memcpy(block + inf, buffer + base, len);


	if (write_block(block_number, block, sb) != 0) {
		loginfo("inode_write: couldn't write a block");
		flogdebug("inode_write: inode %d, block offset %u", index, block_offset);
		free(block);
		return bytes_written;
	}

	bytes_written += len;
	base += bytes_written;
	size_left -= len;

	while (size_left > 0) {
		block_offset += 1;
		block_number = inode_get_block_number(&inode, block_offset, sb);

		if (block_number < 0) {
			logdebug("inode_write: allocating more space");
			inode_add_block(index, &inode, block_offset, sb);
			block_number = inode_get_block_number(&inode, block_offset, sb);
		}

		if (fetch_block(block_number, block, sb) != 0) {
			loginfo("inode_write: couldn't fetch a block");
			flogdebug("inode_write: inode %d, block offset %u", index, block_offset);
			flogdebug("inode_write: block_number %d, offset %u", block_number, inf);
			flogdebug("inode_write: len: %u", len);
			free(block);
			return bytes_written;
		}

		inf = 0;
		len = umin(size_left, sb->blockSize*SECTOR_SIZE);
		memcpy(block + inf, buffer + base, len);

		if (write_block(block_number, block, sb) != 0) {
			loginfo("inode_write: couldn't write a block");
			flogdebug("inode_write: inode %d, block offset %u", index, block_offset);
			free(block);
			return bytes_written;
		}

		bytes_written += len;
		base += bytes_written;
		size_left -= len;
	}

	free(block);

	return bytes_written;
}

///**
// * inode_add_block() - add block to inode
// * @inode: inode struct
// * @index: inode index
// * @offset: block offset inside inode
// * @sb: pointer to superblock structure
// *
// * Add block to inode at block offset `offset`.
// *
// * Return: 0 if succeeds, -1 if there are no more blocks in the disk, -2 if
// * there is an invalid pointer o the way, -3 if offset is off limits.
// */
int inode_add_block(
	int index,
	struct t2fs_inode *inode,
	unsigned int offset,
	struct t2fs_superbloco *sb
) {
	unsigned int dir = NUM_INODE_DIRECT_PTRS;
	unsigned int sind = NUM_INODE_SINGLE_IND_PTRS;
	unsigned int dind = NUM_INODE_DOUBLE_IND_PTRS;
	unsigned int ppb = sb->blockSize*SECTOR_SIZE/PTR_BYTE_SIZE;

	if (offset < dir) {
		return inode_add_block_direct(index, inode, offset, sb);
	} else if (offset < dir + sind*ppb) {
		return inode_add_block_single_ind(index, inode, offset - dir, sb);
	} else if (offset < dir + sind*ppb + dind*ppb*ppb) {
		return inode_add_block_double_ind(index, inode, offset - dir - sind*ppb, sb);
	} else {
		logwarning("inode_add_block: invalid offset");
		return -3;
	}
}

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
) {
	unsigned int data_block_number = new_index_block(sb);

	if (data_block_number < 0) {
		return -1;
	}

	inode->dataPtr[offset] = data_block_number;

	return update_inode(index, inode, sb);
}

int inode_add_block_single_ind(
	int index,
	struct t2fs_inode *inode,
	unsigned int offset,
	struct t2fs_superbloco *sb
) {
	if (inode->singleIndPtr == INVALID_PTR) {
		inode->singleIndPtr = new_index_block(sb);
	}

	int data_block_number = new_index_block(sb);

	if (data_block_number < 0) {
		return -1;
	}

	if (add_pointer_to_index_block(
		inode->singleIndPtr,
		data_block_number,
		offset,
		sb
	) != 0) {
		return -1;
	}


	int err = update_inode(index, inode, sb);
	return err;
}

int inode_add_block_double_ind(
	int index,
	struct t2fs_inode *inode,
	unsigned int offset,
	struct t2fs_superbloco *sb
) {
	if (inode->doubleIndPtr == INVALID_PTR) {
		inode->doubleIndPtr = new_index_block(sb);
	}

	unsigned int ppb = sb->blockSize*SECTOR_SIZE/PTR_BYTE_SIZE;
	int next = inode_follow_once(offset/ppb, inode->doubleIndPtr, sb);
	if (next == -2) {
		logerror("inode_add_block_double_ind: next pointer is -2");
		return -1;
	}

	if (next == INVALID_PTR) {
		next = new_index_block(sb);
		if (next < 0 || add_pointer_to_index_block(
			inode->doubleIndPtr,
			next,
			offset/ppb,
			sb
		) != 0) {
			return -1;
		}
	}

	int data_block_number = new_index_block(sb);

	if (data_block_number < 0) {
		return -1;
	}

	if (add_pointer_to_index_block(
		next,
		data_block_number,
		offset%ppb,
		sb
	) != 0) {
		return -1;
	}


	int err = update_inode(index, inode, sb);
	return err;
}

/* update inode in disk. returns 0 if successful and -1 otherwise. */
int update_inode(
	int index,
	struct t2fs_inode *inode,
	struct t2fs_superbloco *sb
) {
	return write_inode(index, inode, sb);
}

int add_pointer_to_index_block(
	int index_block_number,
	int data_block_number,
	unsigned int offset,
	struct t2fs_superbloco *sb
){
	BYTE *iblock = alloc_buffer(sb->blockSize);

	if (fetch_block(index_block_number, iblock, sb) != 0) {
		logerror("add_pointer_to_index_block: fetching block");
		return -1;
	}

	BYTE *ptr = (BYTE *)malloc(PTR_BYTE_SIZE*sizeof(BYTE));
	int_to_bytes(data_block_number, ptr);
	memcpy(iblock + offset*PTR_BYTE_SIZE, ptr, PTR_BYTE_SIZE);

	if (write_block(index_block_number, iblock, sb) != 0) {
		logerror("add_pointer_to_index_block: writing block");
		return -1;
	}

	free(iblock);
	free(ptr);

	return 0;
}
