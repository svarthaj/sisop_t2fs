#include <stdlib.h>
#include <string.h>
#include "logging.h"
#include "apidisk.h"
#include "t2fs.h"
#include "utils.h"
#include "disk.h"
#include "open_file.h"
#include "list.h"

FILE2 num_handles = -1;

struct list *open_files = NULL;

FILE2 create_open_file(
    char *filename,
    struct t2fs_record *full_record,
    int parent_inode_index,
    unsigned int record_offset
) {
    if(open_files == NULL){
        logdebug("create_open_file. creating open_files list");
        open_files = create_list();
    }

    first_list(open_files);
    while(open_files->it != NULL){
        struct open_file *aux;
        aux = getnode_list(open_files);
		if (strcmp(aux->filename, filename) == 0)
        {
            logwarning("create_open_file. file already open. aborting");
            return -1;
        }

        next_list(open_files);
    }

    num_handles++;

    struct open_file *new = (struct open_file*)malloc(sizeof(struct open_file));
	new->filename = (char *)malloc((1 + strlen(filename))*sizeof(char));
	strcpy(new->filename, filename);
    new->handle = num_handles;
    new->file_offset = 0;
    new->parent_inode_index = parent_inode_index;
    new->record_offset = record_offset;
    new->full_record = *full_record;

    append_list(open_files, new);
    first_list(open_files);

    return new->handle;
}

int destroy_open_file(FILE2 handle){
    if(open_files == NULL){
        logwarning("destroy_open_file. no file open, cannot destroy handle");
        return -1;
    }

    first_list(open_files);
    while(open_files->it != NULL){
        struct open_file *aux;
        aux = getnode_list(open_files);
        if( aux->handle == handle ) {
            remove_list(open_files);
			free(aux->filename);
			free(aux);
            return 0;
        }

        next_list(open_files);
    }

    logwarning("destroy open_file. file not opened. cannot destroy");

    return -1;
}

int get_open_file(FILE2 handle, struct open_file **file) {
    if(open_files == NULL){
        logwarning("get_open_file: no open files");
		*file = NULL;
		return -1;
    }

    first_list(open_files);
    while(open_files->it != NULL){
        struct open_file *aux;
        aux = getnode_list(open_files);
		if (aux->handle == handle)
        {
			*file = aux;
            return 0;
        }

        next_list(open_files);
    }

	logdebug("get_open_file: file handle not found");
	*file = NULL;
	return -1;
}
