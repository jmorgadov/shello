#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

typedef struct linked_list_node{
    void* item;
    void* next;
    void* prev;
}linked_list_node_t;

typedef struct linked_list{
    linked_list_node_t* first;
    linked_list_node_t* last;
    int count;
}linked_list_t;

typedef struct my_stack{
    linked_list_t* items;
}my_stack_t;

linked_list_t* llinit();
linked_list_node_t* llninit(void* value);
void lladd(linked_list_t* llist, void* value);
void* pop_last(linked_list_t* llist);
void* pop_first(linked_list_t* llist);
void push(my_stack_t* stack, void* item);
void* pop(my_stack_t* stack);
my_stack_t* stackinit();

#endif