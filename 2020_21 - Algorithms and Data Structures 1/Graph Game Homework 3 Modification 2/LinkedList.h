/*
    Singly Linked List.
*/

#ifndef LINKED_LIST_H
#define LINKED_LIST_H


// Singly linked list node
typedef struct LinkedListNode {
    struct LinkedListNode *next;

    void *data;
} Node;


typedef struct LinkedList {
    struct LinkedListNode *head;
    struct LinkedListNode *tail;
    int length;

    // (Optional) Destructor for data stored in each node:
    void (* destructor_data) (void *data);

    // optional data's comparison functions
    short (* isequal_data) (const void *data1, const void *data2);
} LinkedList;


// Linked List Functions:

LinkedList* initLinkedList();
void destroyLinkedList(LinkedList *list);
short setDataFunctionsLL(LinkedList *self,
                         short (* const isequal) (const void *data1, const void *data2),
                         void (* const destructor) (void *data));
                       
short isEqualNodeData(const LinkedList *list, const void *data1, const void *data2);
short areEqualNodes(const LinkedList *list, const Node *x, const Node *y);

int appendNode(LinkedList *self, Node *new_node);
int appendValue(LinkedList *self, const void *data);

Node* popFront(LinkedList *self);
short popBack(LinkedList *self);
Node* deleteLinkedListNode(LinkedList *self, Node *to_delete);
Node* findValue(const LinkedList *self, const void *data);
int removeValues(LinkedList *self, const void *data);


#endif
