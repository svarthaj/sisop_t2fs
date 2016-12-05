#include <assert.h>
#include <stdlib.h>
#include "bitmap2.h"
#include "logging.h"
#include "disk.h"
#include "utils.h"
#include "inode.h"
#include "t2fs.h"

static struct t2fs_superbloco get_suberblock() {
	struct t2fs_superbloco sb;

	if (fetch_superblock(&sb) != 0) {
		logerror("test_fetch_inode: superblock fetch");
		exit(1);
	}

	return sb;
}

void test_inode_follow_once() {
	struct t2fs_superbloco sb = get_suberblock();

	inode_follow_once(0, 10, &sb);
}

void test_inode_follow_twice() {
	struct t2fs_superbloco sb = get_suberblock();

	inode_follow_twice(0, 10, &sb);
}

void test_inode_get_block_number() {
	struct t2fs_superbloco sb = get_suberblock();
	struct t2fs_inode inode;

	inode.dataPtr[0] = 100;
	inode.dataPtr[1] = INVALID_PTR;

	unsigned int ppb = sb.blockSize*SECTOR_SIZE/PTR_BYTE_SIZE;

	unsigned int second = 1;
	unsigned int last = (2 + ppb + ppb*ppb) - 1;
	unsigned int past = last + 1;

	assert(inode_get_block_number(&inode, 0, &sb) == 100);
	assert(inode_get_block_number(&inode, second, &sb) == -2);
	//assert(inode_get_block_number(&inode, last, &sb) == -2);
	logwarning("ignore warning below");
	assert(inode_get_block_number(&inode, past, &sb) == -3);
}

void test_inode_read() {
	struct t2fs_superbloco sb = get_suberblock();
	struct t2fs_inode inode;

	inode.dataPtr[0] = 100;
	inode.dataPtr[1] = INVALID_PTR;

	BYTE *buffer = alloc_buffer(sb.blockSize);


	unsigned int ppb = sb.blockSize*SECTOR_SIZE/PTR_BYTE_SIZE;
	unsigned int max_file_size = SECTOR_SIZE*sb.blockSize*(2 + ppb + ppb*ppb);

	int first = 0;
	int past = sb.inodeAreaSize*(SECTOR_SIZE/INODE_BYTE_SIZE);
	assert(inode_read(first, 0, 10, buffer, &sb) == 10);
	logwarning("ignore warning below");
	assert(inode_read(-1, 0, 10, buffer, &sb) == -1);
	logwarning("ignore warning below");
	assert(inode_read(past, 0, 10, buffer, &sb) == -1);
	logwarning("ignore warning below");
	assert(inode_read(first, max_file_size, 10, buffer, &sb) == -1);

	free(buffer);
}

void test_inode_find_record() {
	struct t2fs_superbloco sb = get_suberblock();
	unsigned int offset;
	struct t2fs_record record;

	logdebug("should reach end of inode");
	assert(inode_find_record(0, "poqpoqwpoqwqwpopoqw", &offset, &record, &sb) == -1);
	assert(record.TypeVal == 0x00);
	logwarning("ignore warning below");
	assert(inode_find_record(-1, "arq", &offset, &record, &sb) == -1);
	assert(inode_find_record(0, "arq", &offset, &record, &sb) == 0);
	assert(record.TypeVal == 0x01);
	assert(offset == 0);
	assert(inode_find_record(0, "sub", &offset, &record, &sb) == 0);
	assert(record.TypeVal == 0x02);
	assert(offset == RECORD_BYTE_SIZE);
}

void test_inode_find_free_record() {
	struct t2fs_superbloco sb = get_suberblock();
	unsigned int offset;

	assert(inode_find_free_record(0, &offset, &sb) == 0);
	assert(offset == 2*RECORD_BYTE_SIZE);
	logwarning("ignore two warnings below");
	assert(inode_find_free_record(1239, &offset, &sb) == -1);
}

void test_new_free_inode() {
	assert(getBitmap2(BITMAP_INODE, 0) == 1);
	int i = new_inode();
	assert(getBitmap2(BITMAP_INODE, i) == 1);
	free_inode(i);
	assert(getBitmap2(BITMAP_INODE, i) == 0);
}

int main(int argc, const char *argv[])
{
	test_inode_follow_once();
	test_inode_follow_twice();
	test_inode_get_block_number();
	test_inode_find_record();
	test_inode_find_free_record();
	test_new_free_inode();

	return 0;
}
