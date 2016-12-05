#include <stdlib.h>
#include "logging.h"
#include "apidisk.h"
#include "t2fs.h"
#include "utils.h"
#include "disk.h"
#include "list.h"

struct list *create_list (){
    struct list *new_list;
	new_list = (struct list*)malloc(sizeof(struct list));

    if (new_list){
        new_list->it = NULL;
        new_list->first = NULL;
        new_list->last = NULL;
        return new_list;
	} else {
		logerror("create_list: allocating list");
		exit(1);
	}
}

int first_list (struct list *l){
	if (l == NULL) {
		logwarning("first_list: tried operation on NULL list");
		return -1;
	}

	l->it = l->first;
	return 0;
}

int last_list (struct list *l){
	if (l == NULL) {
		logwarning("last_list: tried operation on NULL list");
		return -1;
	}

	l->it = l->last;
	return 0;
}

int next_list (struct list *l){
	if (l == NULL) {
		logwarning("next_list: tried operation on NULL list");
		return -1;
	}

	if (l->it == NULL) { // end of list or empty list
		return -1;
	} else {
		l->it = l->it->next;
		return 0;
	}
}

void *getnode_list(struct list *l){
	if (l == NULL) {
		logwarning("getnode_list: tried operation on NULL list");
		return NULL;
	}

	if (l->it == NULL) {
		logwarning("getnode_list: tried operation at a NULL iterator");
		return NULL;
	}

	return l->it->node;
}

int append_list (struct list *l, void *data){
	if (l == NULL) {
		logwarning("append_list: tried appending to a NULL list");
		return -1;
	}

    struct node_list *new;
    new = (struct node_list*)malloc(sizeof(struct node_list));

	if (new == NULL) {
		logerror("append_list: allocating new node");
		return -1;
	}

	new->node = data;

	if(l->first == NULL) { // empty list
		l->first = new;
		new->prev = NULL;
	} else { // non empty list
		l->last->next = new;
		new->prev = l->last;
	}

	new->next = NULL;
	l->last = new;
	l->it = l->last;

	return 0;
}

int remove_list (struct list *l){
	if (l == NULL) {
		logwarning("remove_list: tried removing from a NULL list");
		return -1;
	}

	if(l->it == NULL) {
		logdebug("remove_list: tried removing at NULL iterator");
		return -1;
	}

	if(l->it->prev == NULL && l->it->next == NULL){//removing single node
		l->first = NULL;
		l->last = NULL;
	}
	else if(l->it->prev == NULL){//removing first node
		l->first = l->it->next;
		l->first->prev = NULL;
	}
	else if(l->it->next == NULL){//removing last node
		l->last = l->it->prev;
		l->last->next = NULL;
	}
	else{//removing in the middle
		l->it->prev->next = l->it->next;
		l->it->next->prev = l->it->prev;
	}

	free(l->it);
	l->it = l->first;

	return 0;
}

int destroy_list(struct list *l) {
	if (l == NULL) {
		logwarning("destroy_list: tried destroying a NULL list");
		return -1;
	}

	if (l->first == NULL) {
		return 0;
	}

	first_list(l);
	while(l->it != NULL){
		free(l->it->prev);
		next_list(l);
	}
	free(l);

	return 0;
}
