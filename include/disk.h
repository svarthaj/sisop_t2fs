#ifndef DISK_H
#define DISK_H

#include "t2fs.h"

#define RECORD_BYTE_SIZE 64
#define PTR_BYTE_SIZE 4
#define INODE_BYTE_SIZE 16

/**
 * fetch_superblock() - fill superblock structure
 * @sb: pointer to the superblock structure
 *
 * Read the superblock section of the disk and fill the superblock structure
 * accordingly.
 *
 * Return:
 */
int fetch_superblock(struct t2fs_superbloco *sb);

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
);

/**
 * fetch_block() - fill a buffer with the contents of a block
 * @block_number: block number on disk
 * @block: pointer to the buffer
 * @sb: pointer to superblock structure
 *
 * Fill the buffer pointed by `block` with the contents of a block in disk.
 *
 * Return: 0 if succeeds, -1 otherwise.
 */
int fetch_block(
	unsigned int number,
	BYTE *buffer,
	struct t2fs_superbloco *sb
);


/**
 * write_block() - fill a buffer with the contents of a block
 * @block_number: block number on disk
 * @block: pointer to the block
 * @sb: pointer to superblock structure
 *
 * Write the content pointed by `block` on the disk.
 *
 * Return: 0 if succeeds, -1 otherwise.
 */
int write_block(
	unsigned int number,
	BYTE *block,
	struct t2fs_superbloco *sb
);

int new_data_block(struct t2fs_superbloco *sb);

#endif /* ifndef DISK_H */
