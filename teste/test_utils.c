#include <string.h>
#include <assert.h>
#include "logging.h"
#include "utils.h"
#include "list.h"
#include "disk.h"
#include "t2fs.h"

void test_max_min() {
	assert(umax(1, 0) == 1);
	assert(umax(0, 0) == 0);
	assert(umin(1, 0) == 0);
	assert(umin(1, 1) == 1);
}

void test_reverse_endianess() {
	BYTE foo[] = {2, 1, 0};
	BYTE bar[] = {2, 1, 0, 10};

	reverse_endianess(foo, 3);
	reverse_endianess(bar, 3);

	for (int i = 0; i < 3; i++) {
		assert(foo[i] == i);
		assert(bar[i] == i);
	}

	assert(bar[3] == 10);
}

void test_bytes_to_word() {
	BYTE foo[] = {0, 0};
	BYTE bar[] = {2, 0};
	BYTE baz[] = {1, 1};

	assert(bytes_to_word(foo) == 0x0000);
	assert(bytes_to_word(bar) == 0x0002);
	assert(bytes_to_word(baz) == 0x0101);
}

void test_bytes_to_int() {
	BYTE foo[] = {0, 0, 0, 0};
	BYTE bar[] = {2, 0, 0, 0};
	BYTE baz[] = {1, 1, 0, 0};

	assert(bytes_to_int(foo) == 0x00000000);
	assert(bytes_to_int(bar) == 0x00000002);
	assert(bytes_to_int(baz) == 0x00000101);
}

void test_int_to_bytes() {
	BYTE buffer[4];

	for (int i = -200; i < 200; i+= 50) {
		int_to_bytes(i, buffer);
		assert(bytes_to_int(buffer) == i);
	}
}

void test_bytes_to_dword() {
	BYTE foo[] = {0, 0, 0, 0};
	BYTE bar[] = {2, 0, 0, 0};
	BYTE baz[] = {1, 1, 0, 0};

	assert(bytes_to_dword(foo) == 0x00000000);
	assert(bytes_to_dword(bar) == 0x00000002);
	assert(bytes_to_dword(baz) == 0x00000101);
}

void test_bytes_to_record() {
	struct t2fs_record record;
	BYTE buffer[64] = {
		0x01,
		't','e','s','t',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
		1, 1, 0, 0,
		0, 0, 0, 3,
		0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	record = bytes_to_record(buffer);

	assert(record.TypeVal == 0x01);
	assert(record.name[0] == 't');
	assert(record.name[1] == 'e');
	assert(record.name[2] == 's');
	assert(record.name[3] == 't');
	assert(record.blocksFileSize == 0x00000101);
	assert(record.bytesFileSize == 0x03000000);
	assert(record.inodeNumber == 0x00000100);
}

void test_sane_path() {
	logwarning("ignore warning below");
	assert(sane_path("") == -1);
	logwarning("ignore warning below");
	assert(sane_path("/") == -1);
	logwarning("ignore warning below");
	assert(sane_path("/foo//") == -1);
	logwarning("ignore warning below");
	assert(sane_path("//") == -1);
	logwarning("ignore warning below");
	assert(sane_path("/foo/") == -1);
	logwarning("ignore warning below");
	assert(sane_path("/fooooooooooooooooooooooooooooooo") == -1); // 32 chars
	assert(sane_path("/foooooooooooooooooooooooooooooo") == 0); // 31 chars
	assert(sane_path("/foo/bar") == 0);
	assert(sane_path("/foo/bar/baz.txt") == 0);
}

void test_split_path(){
	/* pathname must be declared with [] and not * because it has to be
	   modifiable */
    char pathname[] = "/foo/bar/foobar/filename.txt";

    struct list *path_list = split_path(pathname);

	assert(path_list != NULL);

    assert(strcmp(getnode_list(path_list), "foo") == 0);

    next_list(path_list);
    assert(strcmp(getnode_list(path_list), "bar") == 0);

    next_list(path_list);
    assert(strcmp(getnode_list(path_list), "foobar") == 0);

    next_list(path_list);
    assert(strcmp(getnode_list(path_list), "filename.txt") == 0);

    next_list(path_list);
	logwarning("ignore warning below");
	assert(getnode_list(path_list) == NULL);

	// free(pathname);
	destroy_list(path_list);

    char broken_path[] = "/";
	logwarning("ignore warning below");
    struct list *broken_list = split_path(broken_path);
    assert(broken_list == NULL);
}

void test_find_record() {
	char arq[] = "/arq";
	struct list *names = split_path(arq);
	assert(names != NULL);

	int parent_inode_index;
	unsigned int offset;
	struct t2fs_record record;
	struct t2fs_superbloco sb;
	fetch_superblock(&sb);

	assert(find_record(
		names,
		&record,
		&parent_inode_index,
		&offset,
		&sb
	) == 0);
	assert(record.TypeVal == 0x01);
	assert(strncmp(record.name, "arq", strlen("arq")) == 0);
	assert(parent_inode_index == 0);
	assert(offset == 0);

	char sub[] = "/sub";
	names = split_path(sub);
	assert(names != NULL);

	assert(find_record(
		names,
		&record,
		&parent_inode_index,
		&offset,
		&sb
	) == 0);
	assert(record.TypeVal == 0x02);
	assert(strncmp(record.name, "sub", strlen("sub")) == 0);
	assert(parent_inode_index == 0);
	assert(offset > 0);

	char nonexistent[] = "/no_such_file";
	names = split_path(nonexistent);
	assert(names != NULL);

	logdebug("will reach end of inode");
	assert(find_record(
		names,
		&record,
		&parent_inode_index,
		&offset,
		&sb
	) == 0);
	assert(record.TypeVal == 0x00);
	assert(parent_inode_index == 0);

	char inv[] = "/no/such/path";
	names = split_path(inv);
	assert(names != NULL);

	logdebug("will reach end of inode");
	assert(find_record(
		names,
		&record,
		&parent_inode_index,
		&offset,
		&sb
	) == 0);
	assert(record.TypeVal == 0x00);
	assert(parent_inode_index == -1);
}

int main(int argc, const char *argv[])
{
	test_reverse_endianess();
	test_bytes_to_word();
	test_bytes_to_dword();
	test_bytes_to_int();
	test_int_to_bytes();
	test_max_min();
    test_bytes_to_record();
	test_sane_path();
    test_split_path();
	test_find_record();

    return 0;
}
