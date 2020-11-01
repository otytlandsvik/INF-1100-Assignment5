#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/*
 * List implementation
 */

typedef struct listnode listnode_t;

struct listnode {
    listnode_t  *next;
    void        *item;
};

struct list {
    listnode_t *head;
    int numitems;
};

/*
 * Returns a newly created, empty list.
 */
list_t *list_create(void)
{

    list_t *newListptr = malloc(sizeof(list_t)); // Allocate space for our new list
    if (newListptr == NULL) { // Return NULL to indicate we could not allocate space
        return NULL;
    }
    
    // Initialize our list explicitly
    newListptr->head = NULL;
    newListptr->numitems = 0;
    

    return newListptr;
    
}

/*
 * Frees the list; list and nodes, but not the items it holds.
 */
void list_destroy(list_t *list)
{
    
    
    listnode_t *current = list->head; // Current node we are operating on
    listnode_t *next;
    if (current != NULL) { // We must check whether the head is NULL, because NULL->next will crash
        next = current->next; // Next node
    }
    
    
    // Loop through all nodes
    while (current != NULL) {
        
        free(current); // Free the current node

        current = next; // Iterate to the next node
        if (current != NULL) { // Check wether current is NULL, because NULL->next will crash
            next = current->next; // Iterate the pointer to the next next node
        }

    }

    free(list); // Free the now empty list
    
}

/*
 * Adds an item first in the provided list.
 */
void list_addfirst(list_t *list, void *item)
{
    listnode_t *newHeadptr = malloc(sizeof(listnode_t)); // Allocate space for our new node
    if (newHeadptr == NULL) { // Return immediately if we could not allocate space
        return;
    }
    
    newHeadptr->item = item; // Insert the new item
    newHeadptr->next = list->head; // Set new node to point to the current head

    list->head = newHeadptr; // Set the new node as the head in the list
    list->numitems++; // Increment the number of items in the list
}

/*
 * Adds an item last in the provided list.
 */
void list_addlast(list_t *list, void *item)
{
    listnode_t *newTailptr = malloc(sizeof(listnode_t)); // Allocate space for our new node
    if (newTailptr == NULL) { // Return immediately if we could not allocate space
        return;
    }
    
    newTailptr->item = item; // Insert the new item
    newTailptr->next = NULL; // Since our new node will be the tail, it should point to NULL

    listnode_t *node = list->head; // A new node pointer to iterate through our list
    list->numitems++; // Increment the number of nodes

    if (list->head == NULL) { // If the list is empty set our new node as the head and return
        list->head = newTailptr;
        return;
    }

    while (node->next != NULL) { // Iterate through the nodes until we reach the last
        node = node->next;
    }

    node->next = newTailptr; // Insert the new node after the current tail

}

/*
 * Removes an item from the provided list, only freeing the node.
 */
void list_remove(list_t *list, void *item)
{

    listnode_t *current = list->head; // Current node we are operating on
    listnode_t *previous;
    
    if (current->item == item) { // If head holds our item
        list->head = current->next; // The second item is our new head
        free(current);

        list->numitems--;
        return;
    }

    while (current != NULL) {
        if (current->item == item) {
            previous->next = current->next; // Previous node points to the next node
            free(current); // Free the current node

            list->numitems--;
            return;
        }
        // Iterate to next node
        previous = current;
        current = current->next;
    }
    // If the item is not in our list, we do nothing
}

/*
 * Return the number of items in the list.
 */
int list_size(list_t *list)
{
    return list->numitems;
}



/*
 * Iterator implementation
 */
 

struct list_iterator {
    listnode_t *next;
    list_t *list;
};


/*
 * Return a newly created list iterator for the given list.
 */
list_iterator_t *list_createiterator(list_t *list)
{
    list_iterator_t *newIteratorptr = malloc(sizeof(list_iterator_t)); // Allocate space for our new iterator
    if (newIteratorptr == NULL) { // Return NULL to indicate we could not allocate space
        return NULL;
    }
    
    // Initialize and return our iterator
    newIteratorptr->list = list;
    newIteratorptr->next = list->head;
    return newIteratorptr;
}


/*
 * Free the memory for the given list iterator.
 */
void list_destroyiterator(list_iterator_t *iter)
{
    free(iter);
}


/*
 * Move iterator to next item in list and return current.
 */
void *list_next(list_iterator_t *iter)
{
    // Our current node
    listnode_t *current = iter->next;
    if (current == NULL) { // We cannot move the iterator if current is the NULL pointer
        return NULL;
    }

    // Iterate and return item
    iter->next = iter->next->next;
    return current->item;
}


/*
 * Let iterator point to first item in list again.
 */
void list_resetiterator(list_iterator_t *iter)
{
    iter->next = iter->list->head;
}
