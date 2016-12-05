#include <stdlib.h>
#include "logging.h"
#include "apidisk.h"
#include "t2fs.h"
#include "utils.h"
#include "disk.h"
#include "list.h"

int create_list (struct list *l){
    l->it = NULL;
    l->first = NULL;
    l->last = NULL;
}

int first_list (struct list *l){
    if(l->first != NULL){
        l->it = l->first;
        return 0;
    }

    return -1;
}

int last_list (struct list *l){
    l->it = l->last;
}

int next_list (struct list *l){
    l->it = l->it->next;
}

void *getnode_list(struct list *l){
    return l->it->node;
}

int append_list (struct list *l, void *data){
    struct node_list *new;
    new = (struct node_list*)malloc(sizeof(struct node_list));

    if(l!=NULL){
        new->node = data;
        new->prev = l->last;
        new->next = NULL;
        if(l->first!=NULL)
            l->last->next = new;
        else l->first = new;
        l->it = new;
        l->last = new;
        return 1;
    }
    else return 0;
}

int remove_list (struct list *l){
    struct node_list *aux;

    if(l!=NULL){
        if(l->it!=NULL){
            aux = l->it;
            if(aux->prev == NULL && aux->next == NULL){//removing single node
                l->it = NULL;
                l->first = NULL;
                l->last = NULL;
            }
            else if(aux->prev == NULL){//removing first node
                l->it = aux->next;
                l->first = aux->next;
                aux->next->prev = NULL;  
            }    
            else if(aux->next == NULL){//removing last node
                l->it = aux->prev;
                l->last = aux->prev;
                aux->prev->next = NULL;
            }
            else{ 
                l->it = aux->next;
                aux->prev->next = aux->next;
                aux->next->prev = aux->prev;
            }
            free(aux);
            return 1;
        }
    }
    else return 0;
}


