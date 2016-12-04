#include <assert.h>
#include "logging.h"
#include "utils.h"
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

int main(int argc, const char *argv[])
{
	test_reverse_endianess();
	test_bytes_to_word();
	test_bytes_to_dword();
	test_bytes_to_int();
	test_max_min();
	test_bytes_to_record();
	return 0;
}
