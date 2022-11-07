#include "LinkedList.h"
#include <stdio.h>


// returns pointer to new LinkedList object
// returns NULL in case of error
LinkedList* initLinkedList() {
    LinkedList *list = (LinkedList*) calloc(1, sizeof(LinkedList));
    if (!list) {
        printf("Oops! Calloc failed you my master :(\n");
        return NULL;
    }
    
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;

    list->destructor_data = NULL;
    list->isequal_data = NULL;

    return list;
}


// frees all nodes as well as passed in reference to self
void destroyLinkedList(LinkedList *self) {
    for (Node *cur = self->head; cur != NULL; ) {
        Node *to_del = cur;
        cur = cur->next;

        if (self->destructor_data) self->destructor_data(to_del->data);
        free(to_del);
    }

    // hacing freed all nodes, now delete the LinkedList object
    free(self);
}


// sets pointers to functions necessary for
// user defined structs to be compared, logged
// and freed properly
// returns 0 on error, 1 on success
short setDataFunctionsLL(LinkedList *self,
                       short (* const isequal) (const void *data1, const void *data2),
                       void (* const destructor) (void *data))
{
    if (!self) return 0;

    self->isequal_data = isequal;
    self->destructor_data = destructor;

    return 1;
}


short isEqualNodeData(const LinkedList *list, const void *data1, const void *data2) {
    if (list->isequal_data) return list->isequal_data(data1, data2);
    return (int) data1 == (int) data2;
}


// returns 1 if given nodes of given linked list
// have the same data, 0 otherwise
short areEqualNodes(const LinkedList *list, const Node *x, const Node *y) {
    if (list->isequal_data) return list->isequal_data(x->data, y->data);
    return (int) x->data == (int) y->data;
}


// adds a given node to the given linked list
// returns -1 in case of error, length of the list
// otherwise
int appendNode(LinkedList *self, Node *new_node) {
    if (!self || !new_node) return -1;

    new_node->next = NULL;
    
    if (self->length > 0) {
        self->tail->next = new_node;
    } else {  // first and only element
        self->head = new_node;
    }

    self->tail = new_node;

    self->length += 1;
    return self->length;
}


// constructs and appends node with the given data
// to the given linked list returns -1 in case of error,
// length of the list otherwise
int appendValue(LinkedList *self, const void *data) {
    if (!self) return -1;

    Node *new_node = (Node*) calloc(1, sizeof(Node));
    if (!new_node) {
        printf("Oops! Calloc failed you my master :(\n");
        return -1;
    }

    new_node->data = data;

    return appendNode(self, new_node);
}


// removes given node from the given linked list
// returns next node, returns NULL in case of error
// or no next node to return
Node* deleteLinkedListNode(LinkedList *self, Node *to_delete) {
    if (!self || !self->length || !to_delete) return NULL;

    if (to_delete == self->head) {
        return popFront(self);
    }

    if (to_delete == self->tail) {
        if (!popBack(self)) {
            printf("Failed to pop back\n");
            return NULL;  // error occurred
        }
        return NULL;  // next node after last is NULL
    }

    // swap the data with the one in front of node being deleted
    // no need to actually swap the data which will be removed
    if (self->destructor_data) self->destructor_data(to_delete->data);

    to_delete->data = to_delete->next->data;
    Node *tmp = to_delete->next;  // node to actually delete
    to_delete->next = to_delete->next->next;
    // tmp's data ptr has be reassigned so no data freeing should be done
    free(tmp);

    self->length -= 1;

    return to_delete;
}


// removes first element
// returns NULL in case of error, ptr to new head otherwise
Node* popFront(LinkedList *self) {
    if (!self || !self->length) return NULL;

    Node *new_head = self->head->next;

    if (self->length == 1) {
        self->tail = NULL;
    }

    // delete the head
    if (self->destructor_data) self->destructor_data(self->head->data);
    free(self->head);

    // reset the head
    self->head = new_head;

    self->length -= 1;

    return self->head;
}


short popBack(LinkedList *self) {
    if (!self || !self->length) return 0;

    if (self->length == 1) {
        popFront(self);
        return 1;
    }

    // get to element before tail
    Node *cur = self->head;
    while (cur->next != self->tail) {
        cur = cur->next;
    }

    // release the tail
    if (self->destructor_data) self->destructor_data(self->tail->data);
    free(self->tail);

    // update the tail
    self->tail = cur;
    self->tail->next = NULL;

    self->length -= 1;

    return 1;
}


// returns pointer to first node with given data
// if such node exists, otherwise NULL
Node* findValue(const LinkedList *self, const void *data) {
    if (!self) return NULL;
    
    Node *cur = self->head;
    for (; cur != NULL; cur = cur->next) {
        if (isEqualNodeData(self, cur->data, data)) {
            break;  // node with searched data was found
        }
    }
    return cur;
}


// removes all nodes with given data from the given list
// returns number of nodes removed
int removeValues(LinkedList *self, const void *data) {
    int num_values_removed = 0;
    for (Node *cur = self->head, *prev = NULL; cur != NULL; ) {
        if (isEqualNodeData(self, cur->data, data)) {
            ++num_values_removed;
            Node *to_del = cur;
            cur = cur->next;
            
            if (self->destructor_data) {
                self->destructor_data(to_del->data);
            }
            free(to_del);

            if (prev) {
                // unlink the freed node
                prev->next = cur;
            } else {  // removed the head of the linked list
                self->head = cur;
            }
        } else {
            prev = cur;
            cur = cur->next;
        }
    }

    // update the length of the list
    self->length -= num_values_removed;
    return num_values_removed;
}

