#include <assert.h>
#include "logging.h"
#include "t2fs.h"

void test_indentify2() {
    char name[256];

	assert(identify2((char *)&name, 256) == 0);
	assert(identify2((char *)&name, 1) == 0);
}

void test_open2(){
    char filename1[] = "/arq";
    char filename2[] = "arq";

    FILE2 arq0 = open2("/arq");
    FILE2 arq = open2("/arq");
    FILE2 broken = open2(filename2);

    assert(arq == -1);
    assert(broken == -1);
    assert(arq0 == 0);
}

int main(int argc, char **argv) {
	test_indentify2();
    test_open2();

    return 0;
}
