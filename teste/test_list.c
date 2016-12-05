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

	destroy_list(generic_list);
}

void test_destroy_list(){
    struct list *generic_list = create_list();
    assert(destroy_list(generic_list) == 0);
	logwarning("ignore warning below");
    assert(destroy_list(NULL) == -1);
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

    destroy_list(generic_list);
}

void test_first_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    struct list *generic_list = create_list();

	assert(first_list(generic_list) == 0);

    append_list(generic_list, data1);
    append_list(generic_list, data2);
    append_list(generic_list, data1);
    first_list(generic_list);
    assert(strcmp(generic_list->it->node, data1) == 0);

    destroy_list(generic_list);
}

void test_last_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    struct list *generic_list = create_list();

	assert(last_list(generic_list) == 0);

    append_list(generic_list, data2);
    append_list(generic_list, data2);
    append_list(generic_list, data1);

    last_list(generic_list);
    assert(strcmp(generic_list->it->node, data1) == 0);

    destroy_list(generic_list);
}

void test_next_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    struct list *generic_list = create_list();

    assert(next_list(generic_list) == -1);

    append_list(generic_list, data1);
    append_list(generic_list, data2);
    append_list(generic_list, data1);

    first_list(generic_list);
    assert(strcmp(generic_list->it->node, data1) == 0);

    next_list(generic_list);
    assert(strcmp(generic_list->it->node, data2) == 0);

    next_list(generic_list);
    assert(strcmp(generic_list->it->node, data1) == 0);

    assert(next_list(generic_list) == 0);
    assert(generic_list->it == NULL);

    assert(next_list(generic_list) == -1);

    destroy_list(generic_list);
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
    data3 = (char*)getnode_list(generic_list);
    assert(strcmp(data3, "foo") == 0);

    next_list(generic_list);
    data3 = (char*)getnode_list(generic_list);
    assert(strcmp(data3, "bar") == 0);

    next_list(generic_list);
    data3 = (char*)getnode_list(generic_list);
    assert(strcmp(data3, "foo") == 0);

	destroy_list(generic_list);
}

void test_remove_list(){
    char *data1 = "foo";
    char *data2 = "bar";
    struct list *generic_list = create_list();

	logdebug("will try to remove from empty list:");
	assert(remove_list(generic_list) == -1);

    append_list(generic_list, data1);
	assert(remove_list(generic_list) == 0);
	logdebug("will try to remove from empty list:");
	assert(remove_list(generic_list) == -1);

    append_list(generic_list, data1);
    append_list(generic_list, data2);
    assert(strcmp(generic_list->it->node, data2) == 0);
    assert(remove_list(generic_list) == 0);
    assert(strcmp(generic_list->it->node, data1) == 0);
    assert(remove_list(generic_list) == 0);
	logdebug("will try to remove from empty list:");
	assert(remove_list(generic_list) == -1);

    append_list(generic_list, data1);
    append_list(generic_list, data2);
    first_list(generic_list);
    assert(remove_list(generic_list) == 0);
    assert(strcmp(generic_list->it->node, data2) == 0);
    assert(remove_list(generic_list) == 0);
	logdebug("will try to remove from empty list:");
	assert(remove_list(generic_list) == -1);

    assert(generic_list->it == NULL);
    assert(generic_list->first == NULL);
    assert(generic_list->last == NULL);

	destroy_list(generic_list);
}

int main(int argc, const char *argv[]) {

    test_create_list();
    test_destroy_list();
    test_append_list();
    test_first_list();
    test_last_list();
    test_next_list();
    test_getnode_list();
    test_remove_list();

    return 0;
}
