#include <stdlib.h>
#include <string.h>
#include "logging.h"
#include "apidisk.h"
#include "t2fs.h"
#include "utils.h"
#include "disk.h"
#include "open_file.h"
#include "list.h"

unsigned int num_handles = -1;

struct list *open_files = NULL;

int create_open_file(
    char *filename,
    struct t2fs_record *full_record,
    unsigned int parent_inode_index,
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
        if( memcmp( aux->full_record->name,
                    filename,
                    sizeof(aux->full_record->name)) == 0 )
        {
            logwarning("create_open_file. file already open. aborting");
            return -1;
        }
    
        next_list(open_files);
    }

    num_handles++;    
    
    struct open_file *new = (struct open_file*)malloc(sizeof(struct open_file));   
    new->handle = num_handles;
    new->file_offset = 0;
    new->parent_inode_index = parent_inode_index;
    new->record_offset = record_offset;
    new->full_record = full_record;
    
    append_list(open_files, new);
    first_list(open_files);
    
    return 0;
}

int destroy_open_file(unsigned int handle){
    if(open_files == NULL){
        logwarning("destroy_open_file. no file open, cannot destroy handle");
        return -1;   
    }

    first_list(open_files);
    while(open_files->it != NULL){
        struct open_file *aux;
        aux = getnode_list(open_files);
        if( aux->handle == handle ) {
            logdebug("closing file");
            remove_list(open_files);
            return 0;
        }
    
        next_list(open_files);
    }
    
    logwarning("destroy open_file. file not opened. cannot destroy");
    
    return -1;
}

