#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "logging.h"
#include "t2fs.h"
#include "list.h"

void test_create_list(){
    struct list *generic_list = create_list();

    assert(generic_list->it == NULL);
    assert(generic_list->first == NULL);
    assert(generic_list->last == NULL);
}

void test_destroy_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    struct list *generic_list = create_list();
    
    append_list(generic_list, data1);
    append_list(generic_list, data2);
    append_list(generic_list, data1);
    
    destroy_list(generic_list);
}

void test_append_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    struct list *generic_list = create_list();
    
    append_list(generic_list, data1);
    assert(generic_list->first->node == data1);
    assert(generic_list->last->node == data1);
    
    append_list(generic_list, data2);
    assert(generic_list->first->node == data1);
    assert(generic_list->last->node == data2);
    
    append_list(generic_list, data1);
    assert(generic_list->first->node == data1);
    assert(generic_list->last->node == data1);
}

void test_first_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    struct list *generic_list = create_list();

    append_list(generic_list, data1);
    append_list(generic_list, data2);
    append_list(generic_list, data1);
    first_list(generic_list);
    assert(strcmp(generic_list->it->node, data1) == 0);
}

void test_last_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    struct list *generic_list = create_list();

    append_list(generic_list, data2);
    append_list(generic_list, data2);
    append_list(generic_list, data1);
    
    first_list(generic_list);
    last_list(generic_list);
    assert(strcmp(generic_list->it->node, data1) == 0);
}

void test_next_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    struct list *generic_list = create_list();

    append_list(generic_list, data1);
    append_list(generic_list, data2);
    append_list(generic_list, data1);
    
    first_list(generic_list);
    next_list(generic_list);
    assert(strcmp(generic_list->it->node, data2) == 0);
}

void test_getnode_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    char *data3;
    struct list *generic_list = create_list();

    append_list(generic_list, data1);
    append_list(generic_list, data2);
    append_list(generic_list, data1);
    
    first_list(generic_list);
    next_list(generic_list);
    data3 = (char*)getnode_list(generic_list);
    assert(strcmp(data3, "bar") == 0);
}

void test_remove_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    char *data3 = "foobar";
    struct list *generic_list = create_list();

    append_list(generic_list, data1);
    append_list(generic_list, data2);
    append_list(generic_list, data3);
    append_list(generic_list, data1);
    
    first_list(generic_list);
    next_list(generic_list);
    remove_list(generic_list);
    assert(strcmp(generic_list->it->node, data1) == 0);

    last_list(generic_list);
    remove_list(generic_list);
    assert(strcmp(generic_list->it->node, data1) == 0);
 
    remove_list(generic_list);
    assert(strcmp(generic_list->it->node, data3) == 0);

    remove_list(generic_list);
    assert(generic_list->it == NULL);
    assert(generic_list->first == NULL);
    assert(generic_list->last == NULL);

}

int main(int argc, const char *argv[]) {

    test_create_list();
    test_append_list();
    test_first_list();
    test_last_list();
    test_next_list();
    test_getnode_list();
    test_remove_list();
    //test_destroy_list();

    return 0;
}
