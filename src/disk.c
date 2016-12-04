#include <stdlib.h>
#include <string.h>
#include "logging.h"
#include "apidisk.h"
#include "t2fs.h"
#include "utils.h"
#include "disk.h"

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

int fetch_superblock (struct t2fs_superbloco *sb) {
    BYTE *buffer = alloc_buffer(1);

    if (read_sector(0, buffer) != 0) {
        logerror("fetch_superblock: reading sector");
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
	unsigned int inode_number,
	struct t2fs_inode *inode,
	struct t2fs_superbloco *sb
) {
	BYTE *buffer = alloc_buffer(1);

	unsigned int inodes_per_sector = (SECTOR_SIZE/INODE_DISK_SIZE);
	unsigned int base = sb->superblockSize + sb->freeBlocksBitmapSize + sb->freeInodeBitmapSize;
	unsigned int sector = base + inode_number/inodes_per_sector;

    if (read_sector(sector, buffer) != 0) {
        logerror("fetch_inode: reading sector");
		exit(1);
    }

	unsigned int offset = (inode_number*INODE_DISK_SIZE)%inodes_per_sector;

	inode->dataPtr[0] = bytes_to_int(buffer + offset);
	inode->dataPtr[1] = bytes_to_int(buffer + offset + PTR_SIZE);
	inode->singleIndPtr = bytes_to_int(buffer + offset + 2*PTR_SIZE);
	inode->doubleIndPtr = bytes_to_int(buffer + offset + 3*PTR_SIZE);

	print_inode(inode);

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
