#include <assert.h>
#include "logging.h"
#include "t2fs.h"

void test_indentify2() {
    char name[256];

	assert(identify2((char *)&name, 256) == 0);
	assert(identify2((char *)&name, 1) == 0);
}

void test_open2_close2(){
    FILE2 arq = open2("/arq");
    FILE2 broken = open2("/none");

    assert(arq >= 0);
    assert(broken == -1);

	assert(close2(arq) == 0);
	logwarning("ignore warning below");
	assert(close2(broken) == -1);
}

void test_opendir2_closedir2(){
    DIR2 sub = opendir2("/sub");
    DIR2 broken = opendir2("/none");

    assert(sub >= 0);
    assert(broken == -1);

	assert(closedir2(sub) == 0);
	logwarning("ignore warning below");
	assert(closedir2(broken) == -1);
}

void test_create2(){
	FILE2 file = create2("/file");

	assert(file >= 0);
	assert(close2(file) == 0);
	assert((file = open2("/file")) >= 0);
	assert(close2(file) == 0);

	logdebug("try creating an existing file");
	assert(create2("/arq") == -1);

	logdebug("try creating on inexisting path");
	assert(create2("/bla/blo") == -1);
}

void test_createdir2() {
	flogdebug("here");
	DIR2 dir = mkdir2("/dir");
	assert(dir >= 0);
	assert(closedir2(dir) == 0);
	assert((dir = opendir2("/dir")) >= 0);
	assert(closedir2(dir) == 0);

	flogdebug("here");
	FILE2 file = create2("/dir/file");

	assert(file >= 0);
	assert(close2(file) == 0);
	assert((file = open2("/dir/file")) >= 0);
	assert(close2(file) == 0);
}

void test_read2() {
	logdebug("--------");
	char buffer[132];
	buffer[131] = '\0';
    FILE2 arq = open2("/arq");
	read2(arq, buffer, 132);
	flogdebug("%s", buffer);
}

int main(int argc, char **argv) {
	test_indentify2();
	test_open2_close2();
	test_opendir2_closedir2();
	test_create2();
	test_read2();
	//test_createdir2();

    return 0;
}
