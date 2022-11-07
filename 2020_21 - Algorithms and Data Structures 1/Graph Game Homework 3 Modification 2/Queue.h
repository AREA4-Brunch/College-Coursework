#ifndef QUEUE_H
#define QUEUE_H


#include "LinkedList.h"
#include <stdio.h>  // NULL
#include <stdlib.h>  // calloc, free


typedef struct Queue {
    LinkedList *queue;
} Queue;


// returns ptr to newly constructed queue oject
// returns NULL on error
Queue* initQueue() {
    Queue *queue = (Queue*) calloc(1, sizeof(Queue));
    if (!queue) return NULL;
    queue->queue = initLinkedList();
    if (!queue->queue) return NULL;
    return queue;
}


void destroyQueue(Queue *self) {
    if (!self) return;
    destroyLinkedList(self->queue);
    free(self);
}


// wrapper for setting data struct in the linked list
// which represents the queue
void setDataFunctionsQueue(Queue *self,
                           short (* const isequal) (const void *data1, const void *data2),
                           void (* const destructor) (void *data))
{
    if (self) setDataFunctionsLL(self->queue, isequal, destructor);
}


short isEmptyQueue(const Queue *self) {
    return self->queue->length == 0;
}


// adds given data to top of queue
void pushQueue(Queue *self, const void *data) {
    appendValue(self->queue, data);
}


// removes top from queue, frees the top
void popQueue(Queue *self) {
    popFront(self->queue);
}


// returns the top element of given queue
void* frontQueue(const Queue *self) {
    return self->queue->head->data;
}



#endif
