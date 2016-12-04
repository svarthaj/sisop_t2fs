#include <assert.h>
#include <stdlib.h>
#include "logging.h"
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
	logwarning("ignore warning bellow");
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

int main(int argc, char **argv) {
	test_fetch_superblock();
	test_fetch_inode();
	test_fetch_block();
    return 0;
}
