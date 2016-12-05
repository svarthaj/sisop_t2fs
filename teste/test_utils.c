#include <string.h>
#include <assert.h>
#include "logging.h"
#include "utils.h"
#include "list.h"
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

void test_split_path(){
    char *pathname = "/foo/bar/foobar/filename.txt";
    struct list *path_list = split_path(pathname);

    assert(strcmp(path_list->it->node, "foo") == 0);
    next_list(path_list);
    assert(strcmp(path_list->it->node, "bar") == 0);
    next_list(path_list);
    assert(strcmp(path_list->it->node, "foobar") == 0);
    next_list(path_list);
    assert(strcmp(path_list->it->node, "filename.txt") == 0);

    char *broken_path = "/";
    struct list *broken_list = split_path(broken_path);

    assert(broken_list->it->node == NULL);

    char *empty_path = "";
    struct list *empty_list = split_path(empty_path);
    assert(empty_list->it->node == NULL);

	destroy_list(path_list);
}

int main(int argc, const char *argv[])
{
	test_reverse_endianess();
	test_bytes_to_word();
	test_bytes_to_dword();
	test_bytes_to_int();
	test_max_min();
    test_bytes_to_record();
//    test_split_path();

    return 0;
}
