#ifndef STACK_H
#define STACK_H


#include "LinkedList.h"
#include <stdio.h>  // NULL
#include <stdlib.h>  // calloc, free


typedef struct Stack {
    LinkedList *stack;
} Stack;


// returns ptr to newly constructed stack oject
// returns NULL on error
Stack* initStack() {
    Stack *stack = (Stack*) calloc(1, sizeof(Stack));
    if (!stack) return NULL;
    stack->stack = initLinkedList();
    if (!stack->stack) return NULL;
    return stack;
}


void destroyStack(Stack *self) {
    if (!self) return;
    destroyLinkedList(self->stack);
    free(self);
}


// wrapper for setting data struct in the linked list
// which represents the stack
void setDataFunctionsStack(Stack *self,
                           short (* const isequal) (const void *data1, const void *data2),
                           void (* const destructor) (void *data))
{
    if (self) setDataFunctionsLL(self->stack, isequal, destructor);
}


short isEmptyStack(const Stack *self) {
    return self->stack->length == 0;
}


// adds given data to top of stack
void pushStack(Stack *self, const void *data) {
    appendValue(self->stack, data);
}


// removes top from stack, frees the top
void popStack(Stack *self) {
    popBack(self->stack);
}


// returns the top element of given stack
void* topStack(const Stack *self) {
    return self->stack->tail->data;
}


#endif
