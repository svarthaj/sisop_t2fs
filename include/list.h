#ifndef LIST_H
#define LIST_H

#include "t2fs.h"

struct node_list {
    void *node;
    struct node_list *prev;
    struct node_list *next;
};

struct list {
    struct node_list *it;
    struct node_list *first;
    struct node_list *last;
};

int create_list (struct list *l);
int first_list (struct list *l);
int last_list (struct list *l);
int next_list (struct list *l);
void *getnode_list(struct list *l);
int append_list (struct list *l, void *data);
int remove_list (struct list *l);

#endif /* ifndef LIST_H */
