#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "logging.h"
#include "open_file.h"
#include "t2fs.h"
#include "list.h"

void test_create_open_file(){
    char *names[3] = { "foo", "bar", "foobar" };

    struct t2fs_record file_record[3];

    for(int i = 0; i < 3; i++ ){
        memcpy(file_record[i].name, names[i], sizeof(file_record[i].name));
    }

    assert(open_files == NULL);

    create_open_file(names[0], file_record, 0, 0);
    struct open_file *aux = getnode_list(open_files);
    assert(aux->handle == 0);
    assert(memcmp(aux->full_record->name, names, sizeof(aux->full_record->name)));
    assert(aux->parent_inode_index == 0);
    assert(aux->record_offset == 0);
    
    create_open_file(names[0], file_record, 0, 0);
    aux = getnode_list(open_files);
    assert(aux->handle == 0);
    assert(memcmp(aux->full_record->name, names, sizeof(aux->full_record->name)));
    assert(aux->parent_inode_index == 0);
    assert(aux->record_offset == 0);

    create_open_file(names[1], file_record+1, 1, 1);
    next_list(open_files);
    aux = getnode_list(open_files);
    assert(aux->handle == 1);
    assert(memcmp(aux->full_record->name, names+1, sizeof(aux->full_record->name)));
    assert(aux->parent_inode_index == 1);
    assert(aux->record_offset == 1);
    
    create_open_file(names[2], file_record+2, 2, 2);
    next_list(open_files);
    next_list(open_files);
    aux = getnode_list(open_files);
    assert(aux->handle == 2);
    assert(memcmp(aux->full_record->name, names+2, sizeof(aux->full_record->name)));
    assert(aux->parent_inode_index == 2);
    assert(aux->record_offset == 2);
}

void test_destroy_open_file(){
    char *names[3] = { "foo", "bar", "foobar" };

    struct t2fs_record file_record[3];

    for(int i = 0; i < 3; i++ ){
        memcpy(file_record[i].name, names[i], sizeof(file_record[i].name));
    }

    create_open_file(names[0], file_record, 0, 0);
    
    create_open_file(names[0], file_record, 0, 0);

    create_open_file(names[1], file_record+1, 1, 1);
    
    create_open_file(names[2], file_record+2, 2, 2);
    
    first_list(open_files);

    
    assert(destroy_open_file(0) == 0);
    assert(destroy_open_file(0) == -1);
    assert(destroy_open_file(2) == 0);
    assert(destroy_open_file(3) == -1);;
    
    create_open_file(names[0], file_record, 0, 0);
    next_list(open_files);
    struct open_file *aux = getnode_list(open_files);
    assert(aux->handle == 3);
    assert(memcmp(aux->full_record->name, names, sizeof(aux->full_record->name)));
    assert(aux->parent_inode_index == 0);
    assert(aux->record_offset == 0);

}

int main(int argc, const char *argv[]) {
    test_create_open_file();
    test_destroy_open_file();

    return 0;
}
