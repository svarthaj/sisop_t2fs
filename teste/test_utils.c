#include <assert.h>
#include "utils.h"
#include "t2fs.h"

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

	assert(bytes_to_word(foo) == 0);
	assert(bytes_to_word(bar) == 2);
	assert(bytes_to_word(baz) == 257);
}

void test_bytes_to_dword() {
	BYTE foo[] = {0, 0, 0, 0};
	BYTE bar[] = {2, 0, 0, 0};
	BYTE baz[] = {1, 1, 0, 0};

	assert(bytes_to_word(foo) == 0);
	assert(bytes_to_word(bar) == 2);
	assert(bytes_to_word(baz) == 257);
}

int main(int argc, const char *argv[])
{
	test_reverse_endianess();
	test_bytes_to_word();
	test_bytes_to_dword();
	return 0;
}
