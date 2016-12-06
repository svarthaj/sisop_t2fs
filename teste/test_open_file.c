#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "logging.h"
#include "open_file.h"
#include "t2fs.h"
#include "list.h"

void test_create_get_open_file(){
	struct t2fs_record rec;
	FILE2 handle = create_open_file("/foo", &rec, 0, 0);

	struct open_file *file;
	assert(get_open_file(handle, &file) == 0);
	assert(strcmp(file->filename, "/foo") == 0);
	logdebug("try to get non open file");
	assert(get_open_file(100, &file) == -1);
}

void test_destroy_open_file(){
	struct t2fs_record rec;
	FILE2 handle = create_open_file("/bar", &rec, 0, 0);

	struct open_file *file;
	assert(destroy_open_file(handle) == 0);
	logdebug("try to get non open file");
	assert(get_open_file(handle, &file) == -1);
	logwarning("try to destroy non open file");
	assert(destroy_open_file(handle) == -1);
}

int main(int argc, const char *argv[]) {
	// lembrar de usar nomes diferentes para os arquivos de cada teste
    test_create_get_open_file();
    test_destroy_open_file();

    return 0;
}
