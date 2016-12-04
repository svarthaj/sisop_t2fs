#include <assert.h>
#include "t2fs.h"

void test_indentify2() {
    char name[256];

	assert(identify2((char *)&name, 256) == 0);
	assert(identify2((char *)&name, 1) == 0);
}

int main(int argc, char **argv) {
	test_indentify2();

    return 0;
}
