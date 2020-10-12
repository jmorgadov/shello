#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "datastructs.h"

linked_list_t* llinit(){
    linked_list_t* ll = (linked_list_t*)malloc(sizeof(linked_list_t));
    ll->count = 0;
    ll->last = NULL;
    ll->first = NULL;
    return ll;
}

linked_list_node_t* llninit(void* value){
    linked_list_node_t* n = (linked_list_node_t*)malloc(sizeof(linked_list_node_t));
    n->item = value;
    n->next = NULL;
    n->prev = NULL;
    return n;
}

my_stack_t* stackinit(){
    my_stack_t* s = (my_stack_t*)malloc(sizeof(my_stack_t));
    s->items = llinit();
    return s;
}

void lladd(linked_list_t* llist, void* value){
    if (llist->count == 0){
        llist->first = llist->last = llninit(value);
    }
    else{
        linked_list_node_t* n = llninit(value);
        llist->last->next = n;
        n->prev = (linked_list_node_t*)llist->last;
        llist->last = n;
    }
    llist->count++;
}

void* pop_last(linked_list_t* llist){
    if (llist->count > 0){
        linked_list_node_t* n = llist->last;
        llist->last = (linked_list_node_t*)llist->last->prev;
        return n->item;
    }
    return NULL;
}

void* pop_first(linked_list_t* llist){
    if (llist->count > 0){
        linked_list_node_t* n = llist->first;
        llist->first = (linked_list_node_t*)llist->first->next;
        return n->item;
    }
    return NULL;
}

void push(my_stack_t* stack, void* item){
    lladd(stack->items, item);
}

void* pop(my_stack_t* stack){
    return pop_last(stack->items);
}