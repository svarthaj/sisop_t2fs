#include <string.h>
#include "logging.h"
#include "open_file.h"
#include "disk.h"
#include "utils.h"
#include "list.h"
#include "t2fs.h"

int identify2 (char *name, int size) {
    char *id = "Lucas Leal (206438)\nLuis Mollmann (206440)";

    strncpy(name, id, size);

    if (strncmp(name, id, size) != 0) {
		logwarning("identify2: couldn't copy correctly");
		return -1;
	}

    return 0;
}

FILE2 generic_open(char *filename, struct list *names){
    struct t2fs_record record;
    int ip;
    unsigned int oe;   
    struct t2fs_superbloco sb;
    
    if(fetch_superblock(&sb) != 0){
        logerror("generic open: superblock fetch");
    }
    
    if(find_record(names, &record, &ip, &oe, &sb) != 0){
        logerror("generic open: record find");
    }

    FILE2 handle = create_open_file(filename, &record, ip, oe);
 
    return handle;

}

FILE2 open2 (char *filename) {
    char buffer[32];
    memcpy(buffer, filename, 32);

    struct list *names = split_path(buffer);
    
    if(names != NULL){
        FILE2 handle = generic_open(filename, names);
        return handle;
    }
    return -1;
}

FILE2 create2 (char *filename){
    return -1;
}

FILE2 delete (char *filename){
    return -1;
}

int generic_close(FILE2 handle){
    return destroy_open_file(handle);
}

int close2 (FILE2 handle){
    return generic_close(handle);
}

int read2 (FILE2 handle, char *buffer, int size){
    struct open_file *this_file;
    int i;
    int of;
    int bytes_read;

    first_list(open_files);
    while(open_files->it != NULL){
        struct open_file *aux;
        aux = getnode_list(open_files);
        if(aux->handle == handle){
            this_file = aux; 
        }
        next_list(open_files);
    }
    
    i = this_file->full_record->inodeNumber;
    of = this_file->file_offset;
    inode_read( i, of, size, buffer, &bytes_read ); // must check parameters
    this_file->file_offset += bytes_read;

    return bytes_read;
}

int write2 (FILE2 handle, char *buffer, int size){
    return -1;
}

int truncate2 (FILE2 handle){
    return -1;
}

int seek2 (FILE2 handle, DWORD offset){
    return -1;
}

int mkdir2 (char *pathname){
    return -1;
}

int rmdir2 (char *pathname){
    return -1;
}

DIR2 opendir2 (char *pathname){
    return -1;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry){
    return -1;
}

int closedir2 (DIR2 handle){
    return -1; 
}
