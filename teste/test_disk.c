#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "logging.h"
#include "utils.h"
#include "disk.h"

void test_fetch_block() {
	struct t2fs_superbloco sb;

    if (fetch_superblock(&sb) != 0) {
		logerror("test_fetch_inode: superblock fetch");
		exit(1);
	}

	BYTE *block = (BYTE *)malloc(sb.blockSize*SECTOR_SIZE*sizeof(BYTE));

	if (!block) {
		logerror("test_fetch_block: allocating block");
	}

	unsigned int num_blocks = (sb.diskSize - sb.superblockSize - sb.freeBlocksBitmapSize - sb.freeInodeBitmapSize - sb.inodeAreaSize)/sb.blockSize;
	unsigned int last = num_blocks - 1;
	unsigned int past = last + 1;

	assert(fetch_block(0, block, &sb) == 0);
	assert(fetch_block(last, block, &sb) == 0);
	logwarning("ignore warning below");
	assert(fetch_block(past, block, &sb) == -1);

	free(block);
}

void test_fetch_inode() {
	struct t2fs_superbloco sb;
	struct t2fs_inode root;

    if (fetch_superblock(&sb) != 0) {
		logerror("test_fetch_inode: superblock fetch");
		exit(1);
	}

	if (fetch_inode(0, &root, &sb) != 0) {
		logerror("test_fetch_inode: inode fetch");
		exit(1);
	}

	assert(root.dataPtr[0] >= 0);

	struct t2fs_inode inode;
	int num_inodes = sb.inodeAreaSize*(SECTOR_SIZE/INODE_BYTE_SIZE);
	logwarning("ignore warning below");
	assert(fetch_inode(-1, &inode, &sb));
	logwarning("ignore warning below");
	assert(fetch_inode(num_inodes, &inode, &sb) == -1);
}

void test_fetch_superblock() {
    struct t2fs_superbloco sb;

    if (fetch_superblock(&sb) != 0) {
		logerror("test_fetch_superblock: superblock fetch");
		exit(1);
	}

	assert(sb.id[0] == 'T');
	assert(sb.id[1] == '2');
	assert(sb.id[2] == 'F');
	assert(sb.id[3] == 'S');
	assert(sb.version == 0x7e02);
	assert(sb.superblockSize == 1);
	assert(sb.freeBlocksBitmapSize == 1);
	assert(sb.freeInodeBitmapSize == 1);
	assert(sb.inodeAreaSize == 125);
	assert(sb.blockSize == 16);
	assert(sb.diskSize == 32768);
}

void test_write_block() {
	struct t2fs_superbloco sb;

    if (fetch_superblock(&sb) != 0) {
		logerror("test_write_block: superblock fetch");
		exit(1);
	}

	BYTE *block_written = (BYTE *)malloc(sb.blockSize*SECTOR_SIZE*sizeof(BYTE));
	BYTE *block_read = (BYTE *)malloc(sb.blockSize*SECTOR_SIZE*sizeof(BYTE));

	if (!block_written) {
		logerror("test_write_block: allocating block");
	}
	if (!block_read) {
		logerror("test_write_block: allocating block");
	}

    // fill block to be written on disk
    for (unsigned int i = 0; i < sb.blockSize; i++){
        memcpy(block_written + i*SECTOR_SIZE, "foobar", SECTOR_SIZE);
    }

	unsigned int num_blocks = (sb.diskSize - sb.superblockSize - sb.freeBlocksBitmapSize - sb.freeInodeBitmapSize - sb.inodeAreaSize)/sb.blockSize;
	unsigned int last = num_blocks - 1;
	unsigned int past = last + 1;

    write_block(0, block_written, &sb);
    fetch_block(0, block_read, &sb);
    assert(memcmp(block_written, block_read, sb.blockSize*SECTOR_SIZE) == 0);

    write_block(last, block_written, &sb);
    fetch_block(last, block_read, &sb);
    assert(memcmp(block_written, block_read, sb.blockSize*SECTOR_SIZE) == 0);

    write_block(past, block_written, &sb);
    fetch_block(past, block_read, &sb);
	logwarning("ignore warning below");
    assert(memcmp(block_written, block_read, sb.blockSize*SECTOR_SIZE) == 0);

	free(block_written);
	free(block_read);

}

void test_new_index_block() {
	struct t2fs_superbloco sb;

    if (fetch_superblock(&sb) != 0) {
		logerror("test_write_block: superblock fetch");
		exit(1);
	}

	int index = new_index_block(&sb);
	BYTE *buffer = alloc_buffer(sb.blockSize);
	fetch_block(index, buffer, &sb);

	for (int i = 0; i < sb.blockSize*SECTOR_SIZE; i += PTR_BYTE_SIZE) {
		assert(bytes_to_int(buffer + i) == INVALID_PTR);
	}

	free(buffer);
}

void test_new_data_block() {
	struct t2fs_superbloco sb;

    if (fetch_superblock(&sb) != 0) {
		logerror("test_write_block: superblock fetch");
		exit(1);
	}

	logerror("ignore error below");
	int allocs = 0;
	while (new_data_block(&sb) > 0) {
		allocs++;
	}
	flogdebug("number of blocks allocated: %d", allocs);
}

int main(int argc, char **argv) {
	test_fetch_superblock();
	test_fetch_inode();
	test_fetch_block();
    test_write_block();
	test_new_index_block();
	test_new_data_block();

    return 0;
}
