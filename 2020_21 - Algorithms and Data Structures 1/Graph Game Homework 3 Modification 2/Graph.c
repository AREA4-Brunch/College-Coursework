#include "Graph.h"


// returns pointer to new Graph object
// returns NULL in case of error
Graph* initGraph(const int num_vertices,
                 short (* edge_equality_cmp) (const Edge *edge1, const Edge *edge2),
                 void (* edge_destructor) (Edge *edge))
{
    if (num_vertices < 0) {
        printf("Invalid number of vertices for graph initialization.\n");
        return NULL;
    }

    Graph *graph = (Graph*) calloc(1, sizeof(Graph));
    if (!graph) {
        printf("Oops! Calloc failed you my master :(\n");
        return NULL;
    }

    graph->adj_list_length = 0;
    graph->adj_list = (LinkedList**) malloc(num_vertices * sizeof(LinkedList*));

    if (!graph->adj_list) {
        printf("Malloc failed you my master.\n");
        return NULL;
    }

    graph->adj_list_length = num_vertices;
    graph->edge_equality_cmp = NULL;
    graph->edge_destructor = NULL;

    setGraphsEdgeStruct(graph, edge_equality_cmp, edge_destructor);

    for (int i = 0; i < num_vertices; ++i) {
        graph->adj_list[i] = initLinkedList();
        if (!graph->adj_list[i]) {
            printf("Failed to allocate space for %d-th linked list in graph's egde list.", i);
            return NULL;
        }
        setDataFunctionsLL(graph->adj_list[i],
                         graph->edge_equality_cmp, graph->edge_destructor);
    }

    return graph;
}


// Sets the destructor and equality operator-like function
// for the edges in linked lists of the graph's edge list
void setGraphsEdgeStruct(Graph *self,
                         short (* edge_equality_cmp) (const Edge *edge1, const Edge *edge2),
                         void (* edge_destructor) (Edge *edge))
{
    if (!self) return;
    self->edge_equality_cmp = edge_equality_cmp;
    self->edge_destructor = edge_destructor;
}


// frees a complete edge list
void destroyGraph(Graph *self) {
    if (!self) return;

    for (int i = 0, n = self->adj_list_length; i < n; ++i) {
        if (self->adj_list[i]) {
            destroyLinkedList(self->adj_list[i]);
        }
    }

    if (self->adj_list) free(self->adj_list);
    free(self);
}


// removes given vertex and all its occurances in edge list
// of the given graph
// returns 1 on success, 0 on failure
short removeVertex(Graph *self, const int vertex_idx) {    
    if (!self) return 0;

    if (vertex_idx < 0 || vertex_idx >= self->adj_list_length) {
        printf("Invalid vertex idx.\n");
        return 0;
    }

    if (!self->adj_list[vertex_idx]) {
        printf("Vertex to delete does not exist.\n");
        return 0;
    }

    // remove all edges in which this vertex was a source
    destroyLinkedList(self->adj_list[vertex_idx]);
    self->adj_list[vertex_idx] = NULL;

    // resize the edge list if possible,
    // find the idx of last non null ptr in edge list
    // and remove all empty space after it
    if (vertex_idx == self->adj_list_length - 1) {  // last el is now null
        int last_existing = self->adj_list_length - 1;
        for (; last_existing >= 0; --last_existing) {
            if (self->adj_list[last_existing]) break;
        }

        // in case all vertices are to be deleted
        if (last_existing == -1) {
            free(self->adj_list);
            self->adj_list = NULL;
            self->adj_list_length = 0;
            return 1;
        } else {  // just reallocate space
            LinkedList **tmp = realloc(self->adj_list,
                                       (last_existing + 1) * sizeof(LinkedList*));
            if (!tmp) {
                printf("Reallocation of edge list failed\n");
                return 0;
            }

            // successfull reallocation
            self->adj_list = tmp;
            self->adj_list_length = 1 + last_existing;
        }
    }

    // construct node data to find and remove from all
    // edge list's linked lists
    Edge *to_remove = (Edge*) calloc(1, sizeof(Edge));
    if (!to_remove) {
        printf("Oops! Calloc failed you my master :(\n");
        return 0;
    }
    to_remove->dest = vertex_idx;

    for (int i = 0; i < self->adj_list_length; ++i) {
        if (self->adj_list[i]) {  // check if vertex exists
            // remove all edges that have vertex_idx as dest
            removeValues(self->adj_list[i], to_remove);
        }
    }

    if (self->edge_destructor) self->edge_destructor(to_remove);
    else free(to_remove);

    return 1;
}


// adds vertex to graph, vertices are 0-indexed values
// returns 1 on success, 0 on failure
short addVertex(Graph *self, const int vertex_idx) {
    if (!self) return 0;

    if (vertex_idx < 0) {
        printf("Invalid vertex idx, non negative integers only.\n");
        return 0;
    }

    // check if larger edge list is neccessary
    if (vertex_idx >= self->adj_list_length) {
        // reallocate into tmp not to loose data in case of 
        // realloc failure
        LinkedList **tmp = realloc(self->adj_list,
                                   (vertex_idx + 1) * sizeof(LinkedList*));
        if (!tmp) {
            printf("\nReallocation of edge list failed\n");
            return 0;
        }
        // successfull reallocation
        self->adj_list = tmp;

        // fill all new uninitialized vertices with NULL
        // marking no such vertices exist
        for (int i = self->adj_list_length; i <= vertex_idx; ++i) {
            self->adj_list[i] = NULL;
        }
        // update the capacity of edge list
        self->adj_list_length = vertex_idx + 1;
    } else if (self->adj_list[vertex_idx]) {
        printf("This vertex already is in a graph.\n");
        return 0;
    }

    // fit the vertex into cur edge list
    self->adj_list[vertex_idx] = initLinkedList();
    if (!self->adj_list[vertex_idx]) {  // failed to initialize
        printf("Failed to initialize linked list for the %d-th vertex.\n", vertex_idx);
        return 0;
    }
    setDataFunctionsLL(self->adj_list[vertex_idx],
                     self->edge_equality_cmp, self->edge_destructor);

    return 1;
}


// creates an unweighted directed edge and adds it to the edge list
// of the given graph
// returns 1 on success, 0 on failure
short addEdge(Graph *self, const int source, const int destination) {
    if (!self) return 0;

    if (source < 0 || destination < 0
     || source >= self->adj_list_length
     || destination >= self->adj_list_length)
    {
        printf("Due to invalid vertex indices failed to add edge.\n");
        printf("Edge list length: %d\n", self->adj_list_length);
        return 0;
    }

    if (!self->adj_list[source]) {
        printf("Given source vertex does not exist.\n");
        return 0;
    }

    if (!self->adj_list[destination]) {
        printf("Given destination vertex does not exist.\n");
        return 0;
    }

    Edge *new_edge = (Edge *) calloc(1, sizeof(Edge));

    if (!new_edge) {
        printf("Oops! Calloc failed you my master :(\n");
        return 0;
    }

    new_edge->dest = destination;
    appendValue(self->adj_list[source], new_edge);

    return 1;
}


short deleteEdge(Graph *self, const int source, const int destination) {
    if (!self) return 0;

    if (source < 0 || destination < 0
     || source >= self->adj_list_length
     || destination >= self->adj_list_length)
    {
        printf("Due to invalid vertex indices failed to delete edge.\n");
        return 0;
    }

    if (!self->adj_list[source]) {
        printf("Given source vertex does not exist.\n");
        return 0;
    }

    if (!self->adj_list[destination]) {
        printf("Given destination vertex does not exist.\n");
        return 0;
    }

    Edge *to_del = (Edge *) calloc(1, sizeof(Edge));
    if (!to_del) {
        printf("Oops! Calloc failed you my master :(\n");
        return 0;
    }
    to_del->dest = destination;

    deleteLinkedListNode(self->adj_list[source],
                         findValue(self->adj_list[source], to_del));

    if (self->edge_destructor) self->edge_destructor(to_del);
    else free(to_del);

    return 1;
}


void displayGraph(const Graph *self) {
    if (!self) {
        printf("Uninitialized graph !?\n");
        return;
    }

    const int n = self->adj_list_length;
    if (n <= 0) {
        printf("Empty graph. No vertices were added.\n");
        return;
    }

    printf("Graph:\n");
    for (int i = 0; i < n; ++i) {  // each source
        // skip if i-th vertex deos not exist
        if (!self->adj_list[i]) continue;
        for (Node *cur = self->adj_list[i]->head;
             cur != NULL; cur = cur->next)  // each destination
        {
            printf("%d -> %d\n", i, ((Edge*) cur->data)->dest);
        }
    }
}
