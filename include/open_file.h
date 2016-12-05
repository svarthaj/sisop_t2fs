#ifndef OPEN_FILE_H
#define OPEN_FILE_H

#include "t2fs.h"

// unique identifier for open files. incremented when file is opened
extern unsigned int num_handles;

// open files list
extern struct list *open_files;

/*
 * Data structure for manageing open files.
 * handle: unique identifier for open file. uses next value store in num_handles
 * file_offset: pointer used for navigation inside the file. starts at 0
 * parent_inode_index: inode index for the parent directory.
 * record_offset: offset of the file t2fs_record in the parent directory
 * full_record: pointer to the t2fs_record entry for the file
*/
struct open_file {  
    unsigned int handle;
    unsigned int file_offset;
    unsigned int parent_inode_index;
    unsigned int record_offset;
    struct t2fs_record *full_record;
};

/*
 * create_open_file() - stores new entry for open_file in the list.
 * @filename: name of the file. used for checking if its already open
 * @full_record: pointer to t2fs_record structure
 * @parent_inode_index: inode index for the parent directory.
 *  used for easy acces
 * @record_offset: offset of the file t2fs_record in the parent dir.
 *
 * Checks if file is not open. Case not, creates new strucutre for open_file
 * and stores it the open_files list. 
 *
 * Returns: 0 if succeds, -1 otherwise.
 */   
int create_open_file(
    char *filename,
    struct t2fs_record *full_record,
    unsigned int parent_inode_index,
    unsigned int record_offset);

/*
 * destroy_open_file() - destroy entry for open_file in the list.
 * @handle: unique identifier of the file
 *
 * Search for `handle` in the open_files list. If file is open, removes it. 
 *
 * Returns: 0 if succeds, -1 otherwise.
 */
int destroy_open_file(
    unsigned int handle);

#endif /* ifndef OPEN_FILE_H */
