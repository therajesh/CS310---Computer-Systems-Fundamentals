/***************
 * Sample code that builds and uses a linked list in C
 *
 * We can't build classes, so this is an illustration of using
 * structs and pointers to build our linked list
 *
 * Andy Exley
 *
 **************/

#include <stdlib.h>
#include <stdio.h>

/* struct for the nodes. Let's build a linked list of floats */
struct node {
    float val;
    struct node* next;
};

/* this typedef sets it up so that we can use 
   "node_t" instead of "struct node" everywhere in our code */
typedef struct node node_t;


/* struct for our list "object". */
struct list {
    node_t* head;
    int count;
};

/* again with typedef so we can use "list_t" */
typedef struct list list_t;

/* initialize a list, this is our "constructor" function */
list_t* list_create() {
    list_t* rlist = malloc(sizeof(list_t));
    rlist->count = 0;
    rlist->head = NULL;
    return rlist;
}

/* add "method" - adds to the end of the list */
void list_add(list_t* mlist, float item) {
    if(mlist->count == 0) {
        mlist->head = malloc(sizeof(node_t));
        mlist->head->val = item;
        mlist->head->next = NULL;
    } else {
        node_t* ptr = mlist->head;
        while(ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = malloc(sizeof(node_t));
        ptr->next->val = item;
        ptr->next->next = NULL;
    }
    mlist->count++;
}

/* remove the item at the given index, return it */
float list_remove(list_t* mlist, int loc) {
    if(loc < 0 || loc >= mlist->count) {
        /* index out of bounds! */
        printf("illegal index!\n");
        return 0.0;
    } 
    
    float rval = 0.0;
    if(loc == 0) {
        rval = mlist->head->val;
        node_t* ptr = mlist->head;
        mlist->head = mlist->head->next;
        free(ptr);
    } else {
        int i;
        node_t* ptr = mlist->head;
        for(i = 0; i < loc-1; i++) {
            ptr = ptr->next;
        }
        node_t* toremove = ptr->next;
        rval = toremove->val;
        ptr->next = ptr->next->next;
        free(toremove);
    }
    mlist->count--;
    return rval;
}

/* print "method" */
void list_print(list_t* mlist) {
    node_t* ptr = mlist->head;
    printf("[ ");
    while(ptr != NULL) {
        printf("%0.2f ", ptr->val);
        ptr = ptr->next;
    }
    printf("]\n");
}


/* simple main function, create a list, add some stuff, print it, remove some stuff, print it */
int main(int argc, char** argv) {
    list_t* mylist = list_create();
    list_add(mylist, 1.2);
    list_add(mylist, 3.4);
    list_add(mylist, 4.25);
    list_add(mylist, 2.082);
    list_print(mylist);

    list_remove(mylist, 0);
    list_print(mylist);
    list_remove(mylist, 1);
    list_print(mylist);

    float fl;
    printf("Type in a number to add: ");
    scanf("%f", &fl);
    
    list_add(mylist, fl);
    list_print(mylist);
}