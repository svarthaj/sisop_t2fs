#include <stdlib.h>
#include <string.h>
#include "bitmap2.h"
#include "logging.h"
#include "apidisk.h"
#include "t2fs.h"
#include "utils.h"
#include "disk.h"

int fetch_superblock (struct t2fs_superbloco *sb) {
    BYTE *buffer = alloc_buffer(1);

    if (read_sector(0, buffer) != 0) {
        logerror("fetch_superblock: reading sector");
		free(buffer);
		exit(1);
    }

    memcpy(&sb->id, buffer, 4);

	sb->version = bytes_to_word(buffer + 4);
    sb->superblockSize = bytes_to_word(buffer + 6);
    sb->freeBlocksBitmapSize = bytes_to_word(buffer + 8);
    sb->freeInodeBitmapSize = bytes_to_word(buffer + 10);
    sb->inodeAreaSize = bytes_to_word(buffer + 12);
    sb->blockSize = bytes_to_word(buffer + 14);
    sb->diskSize = bytes_to_dword(buffer + 16);

	free(buffer);

    return 0;
}

/**
 * fetch_inode() - fill inode structure
 * @inode_number: the number of the inode in the disk
 * @inode: pointer to an inode structure
 * @sb: pointer to a superblock structure
 *
 * Reads the disk section corresponding to given inode number and fill
 * the inode structure pointed by `inode`.
 *
 * Return: 0 if succeeds, -1 otherwise.
 */
int fetch_inode(
	int inode_number,
	struct t2fs_inode *inode,
	struct t2fs_superbloco *sb
) {
	unsigned int inodes_per_sector = (SECTOR_SIZE/INODE_BYTE_SIZE);
	if (inode_number < 0
	    || inode_number >= sb->inodeAreaSize*inodes_per_sector
		|| getBitmap2(BITMAP_INODE, inode_number) == 0) {
		logwarning("fetch_inode: invalid inode number");
		return -1;
	}

	BYTE *buffer = alloc_buffer(1);

	unsigned int base = sb->superblockSize + sb->freeBlocksBitmapSize + sb->freeInodeBitmapSize;
	unsigned int sector = base + inode_number/inodes_per_sector;

    if (read_sector(sector, buffer) != 0) {
        logerror("fetch_inode: reading sector");
		free(buffer);
		return -1;
    }

	unsigned int offset = inode_number%inodes_per_sector;

	inode->dataPtr[0] = bytes_to_int(buffer + offset);
	inode->dataPtr[1] = bytes_to_int(buffer + offset + PTR_BYTE_SIZE);
	inode->singleIndPtr = bytes_to_int(buffer + offset + 2*PTR_BYTE_SIZE);
	inode->doubleIndPtr = bytes_to_int(buffer + offset + 3*PTR_BYTE_SIZE);

	free(buffer);

	return 0;
}

/**
 * write_inode()
 * @inode_number: the number of the inode in the disk
 * @inode: pointer to an inode structure
 * @sb: pointer to a superblock structure
 *
 * Write the disk section corresponding to given inode number and fill
 * it with the inode structure pointed by `inode`.
 *
 * Return: 0 if succeeds, -1 otherwise.
 */
int write_inode(
	int inode_number,
	struct t2fs_inode *inode,
	struct t2fs_superbloco *sb
) {
	unsigned int inodes_per_sector = (SECTOR_SIZE/INODE_BYTE_SIZE);
	if (inode_number < 0
	    || inode_number >= sb->inodeAreaSize*inodes_per_sector
		|| getBitmap2(BITMAP_INODE, inode_number) == 0) {
		logwarning("write_inode: invalid inode number");
		return -1;
	}

	BYTE *buffer = alloc_buffer(1);

	unsigned int base = sb->superblockSize + sb->freeBlocksBitmapSize + sb->freeInodeBitmapSize;
	unsigned int sector = base + inode_number/inodes_per_sector;

    if (read_sector(sector, buffer) != 0) {
        logerror("write_inode: reading sector");
		free(buffer);
		return -1;
    }

	unsigned int offset = inode_number%inodes_per_sector;

	BYTE *ptr_bytes = (BYTE *)malloc(PTR_BYTE_SIZE*sizeof(BYTE));
	int_to_bytes(inode->dataPtr[0], ptr_bytes);
	memcpy(buffer + offset + 0*PTR_BYTE_SIZE, ptr_bytes, PTR_BYTE_SIZE);

	int_to_bytes(inode->dataPtr[1], ptr_bytes);
	memcpy(buffer + offset + 1*PTR_BYTE_SIZE, ptr_bytes, PTR_BYTE_SIZE);

	int_to_bytes(inode->singleIndPtr, ptr_bytes);
	memcpy(buffer + offset + 2*PTR_BYTE_SIZE, ptr_bytes, PTR_BYTE_SIZE);

	int_to_bytes(inode->doubleIndPtr, ptr_bytes);
	memcpy(buffer + offset + 3*PTR_BYTE_SIZE, ptr_bytes, PTR_BYTE_SIZE);

    if (write_sector(sector, buffer) != 0) {
        logerror("write_inode: writing sector");
		free(ptr_bytes);
		free(buffer);
		return -1;
    }

	free(ptr_bytes);
	free(buffer);

	return 0;
}

/**
 * fetch_block() - fetch a block from disk
 * @number: block number
 * @buffer: pointer to buffer where the block will be copied
 * @sb: superblock structure.
 *
 * Reads a block from disk and copies it to a buffer. The buffer has to be
 * at least sb->blockSize.
 *
 * Return: 0 if succeeds, -1 otherwise.
 */
int fetch_block(
	unsigned int number,
	BYTE *buffer,
	struct t2fs_superbloco *sb
) {
	unsigned int base = sb->superblockSize + sb->freeBlocksBitmapSize + sb->freeInodeBitmapSize + sb->inodeAreaSize;
	unsigned int offset = number*sb->blockSize;

	if (base + offset + sb->blockSize > sb->diskSize) {
		logwarning("fetch_block: tried to read past end of disk");
		return -1;
	}

	for (unsigned int i = 0; i < sb->blockSize; i++) {
		if (read_sector(base + offset + i, buffer + i*SECTOR_SIZE) != 0) {
			logerror("fetch_block: reading sector");
			exit(1);
		}
	}

	return 0;
}

int write_block(
    unsigned int number,
    BYTE *block,
    struct t2fs_superbloco *sb
) {
	unsigned int base = sb->superblockSize + sb->freeBlocksBitmapSize + sb->freeInodeBitmapSize + sb->inodeAreaSize;
	unsigned int offset = number*sb->blockSize;

    if (base + offset + sb->blockSize > sb->diskSize) {
		logwarning("write_block: tried to write past end of disk");
		return -1;
	}

	for (unsigned int i = 0; i < sb->blockSize; i++) {
		if (write_sector(base + offset + i, block + i*SECTOR_SIZE) != 0) {
			logerror("fetch_block: writing sector");
			exit(1);
		}
	}

	return 0;
}

/* return the block number or -1 in case of error */
int new_data_block(struct t2fs_superbloco *sb) {
	int s;
	int found = 0;
	while (!found) {
		s = searchBitmap2(BITMAP_DADOS, 0);

		if (s == 0) {
			logerror("new_data_block: searchBitmap failed");
			return -1;
		}

		while (s%16 != 0) {
			int err = setBitmap2(BITMAP_DADOS, s, 1);
			if (err < 0) {
				logerror("new_data_block: setBitmap failed");
			}
			s++;
		}

		found = 1;
		for (int i = 0; i < sb->blockSize; i++) {
			if (getBitmap2(BITMAP_DADOS, s + i) != 0) {
				found = 0;
				break;
			} else {
				int err = setBitmap2(BITMAP_DADOS, s, 1);
				if (err < 0) {
					logerror("new_data_block: setBitmap failed");
				}
			}
		}
	}

	return s/sb->blockSize;
}

/* return the block number or -1 in case of error */
int new_index_block(struct t2fs_superbloco *sb) {
	int block_number = new_data_block(sb);

	if (block_number == -1) {
		return -1;
	}

	BYTE *block = alloc_buffer(sb->blockSize);
	fetch_block(block_number, block, sb);

	BYTE *invalid_ptr_bytes = (BYTE *)malloc(PTR_BYTE_SIZE*sizeof(BYTE));
	if (!invalid_ptr_bytes) {
		logerror("new_index_block: allocating invalid_ptr_bytes");
		exit(1);
	}

	int_to_bytes(INVALID_PTR, invalid_ptr_bytes);

	for (int i = 0; i < sb->blockSize*SECTOR_SIZE; i+= PTR_BYTE_SIZE) {
		memcpy(block + i, invalid_ptr_bytes, PTR_BYTE_SIZE);
	}

	write_block(block_number, block, sb);
	free(block);

	return block_number;
}
