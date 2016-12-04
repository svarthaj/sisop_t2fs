#ifndef DISK_H
#define DISK_H

#include "t2fs.h"

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
 * print_superblock() - print superblock structure to stderr
 * @sb: pointer to the superblock structure
 *
 * Print the fields of a superblock structure to the stderr.
 *
 * Return:
 */
void print_superblock(struct t2fs_superbloco *sb);

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
 * print_inode() - print inode structure to stderr
 * @sb: pointer to the inode structure
 *
 * Print the fields of a inode structure to the stderr.
 *
 * Return:
 */
void print_inode(struct t2fs_inode *inode);

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


#endif /* ifndef DISK_H */
