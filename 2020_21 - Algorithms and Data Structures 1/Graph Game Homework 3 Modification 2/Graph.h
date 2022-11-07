#ifndef GRAPH_H
#define GRAPH_H

#include "LinkedList.h"
#include <stdlib.h>  // free


// ===========================
// Graphs Edge:


typedef struct Edge {
    int dest;   // destinatino of the edge
} Edge;


// destructor for edge ptr
void destroyEdge(Edge *self) {
    if (self) free(self);
}


// returns 1 if given edges are equal, 0 otherwise
short areEqualEdges(const Edge *x, const Edge *y) {
    return x->dest == y->dest;
}


// ===========================
// Graph Itself:

typedef struct Graph {
    LinkedList **adj_list;
    int adj_list_length;

    // Equality check for comparison of edges within linked lists
    short (* edge_equality_cmp) (const Edge *edge1, const Edge *edge2);

    // Destructor for edges:
    void (* edge_destructor) (Edge *edge);
} Graph;



// ===========================
// Graph Functions:


Graph* initGraph(const int num_vertices,
                 short (* edge_equality_cmp) (const Edge *edge1, const Edge *edge2),
                 void (* edge_destructor) (Edge *edge));
void destroyGraph(Graph *self);
void setGraphsEdgeStruct(Graph *self,
                         short (* edge_equality_cmp) (const Edge *edge1, const Edge *edge2),
                         void (* edge_destructor) (Edge *edge));

short addVertex(Graph *self, const int vertex_idx);
short removeVertex(Graph *self, const int vertex_idx);

short addEdge(Graph *self, const int source, const int destination);
short deleteEdge(Graph *self, const int source, const int destination);

void displayGraph(const Graph *self);


#endif
