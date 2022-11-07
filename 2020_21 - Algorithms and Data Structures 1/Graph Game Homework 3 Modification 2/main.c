/*
    Program for:
    1) building and managing a graph
        and
    2) simulating a game:
        1st player (Maja) on each turn moves by 1 square,
        2nd player (Sanja) on every odd turn moves by 1 square.
                        and on every even turn moves by 2 squares
        (for the player's path to be valid there must be as many squares ahead as
        the turn requires)

        The goal is to reach the given square from the same starting point
        in fewest turns.

        Find the shortest path for each player and determine a winner with
        NO RECURSION, strictly iteratively.

    Some test samples for the game (including main menu interaction, etc.):

        I:
            8
            6
            7
            0 1
            2 0
            1 3
            2 3
            4 2
            3 5
            5 4
            0
            5
            21

        II:
            8
            6
            8
            0 1
            2 0
            1 3
            1 5
            2 5
            5 4
            4 2
            3 4
            0
            5
            21

*/

#define _CRT_SECURE_NO_WARNINGS

//     // HERE
// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>  // for INT_MAX

#include "LinkedList.h"
#include "Stack.h"
#include "Queue.h"
#include "Graph.h"


#include "LinkedList.c"
#include "Graph.c"


// ===========================
// Error handling:


const int ERROR_CODE_CALLOC = 1;
const int ERROR_CODE_SCANF = 2;


#define CALLOC_ERROR \
printf("Oops! Calloc failed you my master :(\n"); \
exit(ERROR_CODE_CALLOC);


#define SCANF_ERROR \
printf("Oops! Scanf failed to input :(\n"); \
exit(ERROR_CODE_SCANF);


// ===========================
// Pair of ints data structure


typedef struct PairInt {
    int first;
    int second;
} PairInt;


// constructor for struct PairInt
PairInt* initPairIntEmpty() {
    PairInt *pair = (PairInt*) calloc(1, sizeof(PairInt));
    pair->first = pair->second = 0;
    return pair;
}


// constructs pair with given values and
// returns ptr to it
PairInt* initPairInt(const int first, const int second) {
    PairInt *pair = (PairInt*) calloc(1, sizeof(PairInt));
    pair->first = first;
    pair->second = second;
    return pair;
}


// destructor for struct PairInt
void destroyPairInt(PairInt *self) {
    if (self) free(self);
}


// ===========================
// Graph Trevrsal functions:


int Dijkstra(const Graph *graph, const int start, const int end);
int getMinsIdx(const short *has_processed, const int *dists,
               const int num_vertices);

void displayDijkstraPath(const int start, const int end, const int *prevs);
void logShortestPathsNoCycles(const Graph *graph, const int start, const int end,
                      const int path_length);

int findSanjasShortestPath(const Graph *graph, const int start, const int end);
void logSanjasShortestPaths(const Graph *graph, const int start, const int end,
                            const int num_moves);
int calcSanjasMoveIdx(const int vertex_path_idx);


// ===========================
// Program functions:


short simulateGame();
void wasteUsersTime();
void displayMainMenu();

short loadEdges(Graph *graph);


void testQueue();
void testStack();
void tmpTest();
void testLinkedList();
void displayEdgeLinkedList(const LinkedList *list);
void displayIntLinkedList(const LinkedList *list);
void displayPairIntLinkedList(const LinkedList *list);


// ===========================
// Helper Functions:


short readInt(int *x, const char *msg);



// ===========================
// RUNNER PROGRAM


int main()
{
    // HERE
    // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // freopen("test.txt", "r", stdin);

    // testLinkedList();
    // tmpTest();
    // testQueue();

    wasteUsersTime();

    // HERE
    // _CrtDumpMemoryLeaks();

    return 0;
}



// ====================================
// Functions that interact with user:


void wasteUsersTime() {
    short do_quit = 0;

    Graph *users_graph = NULL;

    while (!do_quit) {

        displayMainMenu();

        int user_cmd;
        if(scanf("%d", &user_cmd) != 1) {
            printf("\nThere was an input error! Please try again\n");
            continue;
        }

        int vertex, e;

        switch (user_cmd) {
            case 1:  // Initialize a graph
                printf("Enter the number of vertices: ");
                int n;  if(scanf("%d", &n) != 1) { SCANF_ERROR };
                if (users_graph) {
                    destroyGraph(users_graph);
                    printf("\nDestroying and overwriting existing graph.\n");
                }
                users_graph = initGraph(n, areEqualEdges, destroyEdge);
                if (!users_graph) {
                    printf("\nFailed to initialize the graph\n");
                }
                break;

            case 2:  // Destroy the graph
                if (users_graph) destroyGraph(users_graph);
                users_graph = NULL;
                break;

            case 3:  // Display the graph
                displayGraph(users_graph);
                break;

            case 4:  // Add vertex to graph
                printf("Enter the id of vertex to add(non-negative int): ");
                if(scanf("%d", &vertex) != 1) { SCANF_ERROR; }
                if (!addVertex(users_graph, vertex)) {
                    printf("\nFailed to add a vertex\n");
                }
                break;

            case 5:  // Add edge to graph
                loadEdges(users_graph);
                break;

            case 6:  // Delete vertex from graph
                printf("Enter the id of vertex to remove(non-negative int): ");
                if(scanf("%d", &vertex) != 1) { SCANF_ERROR; }
                if (!removeVertex(users_graph, vertex)) {
                    printf("\nFailed to remove a vertex\n");
                }
                break;

            case 7:  // Delete edge from graph
                printf("Enter number of edges to delete: ");
                if(scanf("%d", &e) != 1) { SCANF_ERROR; }
                printf("Enter the edges in format: `source destination`; vertices are non-negative ints:\n");
                for (int i = 0; i < e; ++i) {
                    int source, dest;
                    if (scanf("%d %d", &source, &dest) != 2) { SCANF_ERROR; }
                    if (!deleteEdge(users_graph, source, dest)) {
                        printf("\nFailed to delete an edge. Keep entering edges to delete:\n");
                    }
                }
                break;

            case 8:  // Simulate the game
                if (!simulateGame()) {
                    printf("\n\nAn error during simulation occurred.\nPlease try again.\n");
                }
                break;

            default:
                do_quit = 1;
                break;
        }  // endswitch

    }  // endwhile

    if (users_graph) {
        destroyGraph(users_graph);
    }

}


void displayMainMenu() {
    printf("\n\nPlease choose one of the following\n(type in the option's number and press Enter):\n\n");
    printf("1 Initialize a graph\n");
    printf("2 Destroy the graph\n");
    printf("3 Display the graph\n");
    printf("4 Add vertex to graph\n");
    printf("5 Add edges to graph\n");
    printf("6 Delete vertex from graph\n");
    printf("7 Delete edges from graph\n");
    printf("8 Simulate the game\n");
    printf("\n\nEnter any other integer to Quit\n\n");
}


// returns 1 on success, 0 in case of error
short simulateGame() {

    // Enter the board:
    int num_playing_fields;
    if (!readInt(&num_playing_fields, "Enter number of fields on the board")) return 0;

    Graph *board = initGraph(num_playing_fields, areEqualEdges, destroyEdge);
    if (!board) {
        printf("\nFailed to initialize the graph.\n");
        return 0;
    }

    loadEdges(board);

    // Get start and finish
    int start, finish;
    if (!readInt(&start, "Enter start point")) return 0;
    if (!readInt(&finish, "Enter finish point")) return 0;

    // Maja's shortest path ios just straight from start to
    // finish, therefore number of moves is the same as the num
    // of edges on her path
    const int majas_shortest_path_length = Dijkstra(board, start, finish);
    const int majas_min_num_of_moves = majas_shortest_path_length;

    if (majas_shortest_path_length < 0) {
        printf("\nThere was an error seacrhing for path from %d to %d\n", start, finish);
        return 0;
    }

    if (majas_shortest_path_length == INT_MAX) {
        printf("\nFinish %d is unreachable from start: %d\n", finish, start);
        return 1;
    }

    // Sanja can reach the end of path without additional moves left to spend
    // if such path's number of edges has remainder 2 when divided by 3
    // otherwise a more complex path needs to be constructed
    int sanjas_shortest_path_num_edges = -1;
    int sanjas_min_num_of_moves = -1;
    if (majas_shortest_path_length % 3 != 2) {
        sanjas_shortest_path_num_edges = majas_shortest_path_length;
        sanjas_min_num_of_moves = sanjas_shortest_path_num_edges
                                - (sanjas_shortest_path_num_edges / 3);
    } else {
        sanjas_min_num_of_moves = findSanjasShortestPath(board, start, finish);
        if (sanjas_min_num_of_moves == INT_MAX) {
            printf("\nSanja cannot reach %d from start: %d\n", finish, start);
            // return 1;
            // Maja should still be logged
        }
    }

    // log the winner of the game:
    printf("\n\nThe winner is: ");
    if (sanjas_min_num_of_moves < majas_min_num_of_moves) {
        printf("Sanja!\n");
    } else if (majas_min_num_of_moves < sanjas_min_num_of_moves ) {
        printf("Maja!\n");
    } else {
        printf("It's a tie!\n");
    }

    // log the min moves 1st player:
    printf("\nMaja's number of moves: %d\n", majas_min_num_of_moves);
    // log the min moves 2nd player:
    printf("\nSanja's number of moves: %d\n", sanjas_min_num_of_moves);


    // log all paths of length min moves of 1st player:
    printf("\nMaja's paths:\n");
    logShortestPathsNoCycles(board, start, finish, majas_shortest_path_length);

    // log all paths of length min moves of 2nd player:
    printf("\nSanja's paths:\n");
    logSanjasShortestPaths(board, start, finish, sanjas_min_num_of_moves);

    destroyGraph(board);

    return 1;
}


// interface through which user adds edges to given graph
// returns 1 on success, 0 if some error ocurred
// terminates program if more than 5 consequent scanf failures
short loadEdges(Graph *graph) {
    int error_code = 1;
    int e;  // num of edges
    if (!readInt(&e, "Enter number of edges")) return 0;
    printf("Enter the edges in format: `source destination`; vertices are non-negative ints:\n");
    for (int i = 0; i < e; ++i) {
        int source, dest;
        short too_many_failures = 5;
        while (too_many_failures-- && scanf("%d %d", &source, &dest) != 2) {
            printf("Failed to read in data. Please try again.\n");
        }
        if (!too_many_failures) {
            printf("\nToo many scanf failures, terminating.\n");
            SCANF_ERROR;
        }
        if (!addEdge(graph, source, dest)) {
            printf("Failed to add an edge\n");
            error_code = 0;
        }
    }
    return error_code;
}


// prompts user with given msg and
// reads int from stdin into given argument
// logs to stdout msg if error occurres
// returns 1 on success, 0 on error
short readInt(int *x, const char *msg) {
    printf("%s: ", msg);

    if (1 != scanf("%d", x)) {
        printf("\nFailed to read in data.\n");
        // SCANF_ERROR;
        return 0;
    }

    return 1;
}



// =====================================
// Graph Traversal Functions:


// performs Dijkstra in O(|V|^2) with array implementation
// on given unweighted graph
// returns length of shortest path, in case of no path
// returns INT_MAX, -1 on error
int Dijkstra(const Graph *graph, const int start, const int end) {
    if (!graph || !graph->adj_list_length) return -1;

    const int num_vertices = graph->adj_list_length;
    int *prevs = malloc(num_vertices * sizeof(int));
    int *dists = malloc(num_vertices * sizeof(int));
    short *has_processed = (short*) calloc(num_vertices, sizeof(short));

    if (!prevs || !dists || !has_processed) {
        printf("\nAllocation failure in Dijkstra.\n");
        return -1;
    }

    // initialize:
    for (int i = 0; i < num_vertices; ++i) {
        dists[i] = INT_MAX;
        prevs[i] = -1;
        // mark vertices that do not exist as processed
        if (!graph->adj_list[i]) has_processed[i] = 1;
        // else has_processed[i] is already 0
    }

    dists[start] = 0;
    prevs[start] = start;

    for (int cur = getMinsIdx(has_processed, dists, num_vertices);
         cur >= 0;
         cur = getMinsIdx(has_processed, dists, num_vertices))
    {
        has_processed[cur] = 1;

        if (cur == end) {
            // display path
            // displayDijkstraPath(start, end, prevs);
            free(prevs);
            free(has_processed);
            const int min_dist = dists[cur];
            free(dists);
            return min_dist;
        }

        // relax all vertices with cur vertex
        for (Node *it = graph->adj_list[cur]->head;
             it != NULL; it = it->next)
        {
            const int dest_vertex = ((Edge*) it->data)->dest;
            const int weight = 1;  // since this is unweighted graph
            if (dists[cur] + weight < dists[dest_vertex]) {
                dists[dest_vertex] = dists[cur] + weight;
                prevs[dest_vertex] = cur;
            }
        }
    }
    // this will never be reached
}


// returns idx of vertex with min dist which was not processed
// if no such vertex returns -1
int getMinsIdx(const short *has_processed, const int *dists,
               const int num_vertices)
{
    int min_idx = -1;
    int min_val = INT_MAX;
    for (int i = 0; i < num_vertices; ++i) {
        if (has_processed[i]) continue;

        if (dists[i] <= min_val) {
            min_val = dists[i];
            min_idx = i;
        }
    }
    return min_idx;
}


void displayDijkstraPath(const int start, const int end, const int *prevs) {
    if (!prevs) return;
    if (prevs[end] < 0) {
        printf("There is no path from %d to %d\n", start, end);
        return;
    }

    if (start == end) {
        printf("Path from %d to %d:\n%d\n", start, end, start);
        return;
    }

    // get length of the path
    int path_length = 1;  // 1 for the end node
    for (int cur = end; cur != start && cur > 0; cur = prevs[cur]) {
        ++path_length;
    }

    // store the path in natural order, from start to end
    int *path = malloc(path_length * sizeof(int));
    if (!path) {
        printf("There was no more space to store the path.\n");
        return;
    }
    path[path_length - 1] = end;
    for (int i = path_length - 2; i >= 0; --i) {
        path[i] = prevs[path[i + 1]];
    }

    // log the path
    printf("Path from %d to %d:\n", start, end);
    for (int i = 0; i < path_length; ++i) {
        printf("%d ", path[i]);
    }
    printf("\n");
}


// examines all paths from start to end and
// logs to stdout only the ones of given length
// does not allow a single vertex to repeat itself in the final path
void logShortestPathsNoCycles(const Graph *graph, const int start, const int end,
                              const int path_length)
{
    if (!graph || !graph->adj_list_length) return;

    // in case there are no outgoing edges from start:
    if (!graph->adj_list[start]
     || graph->adj_list[start]->length == 0) {
        if (start != end) {
            printf("\n%d is unreachable from %d\n", end, start);
        } else {
            printf("Path:\n%d\n", start);
        }
        return;
    }

    // num of vertices to explore on the path of given lengths
    const int max_path_vertices = 1 + path_length;
    const int num_vertices = graph->adj_list_length;

    short *has_visited = (short*) calloc(num_vertices, sizeof(short));
    // array of vertices forming single path which is being processed
    // 1-indexed (so the inverse sign flagging is possible):
    int *path = (int*) calloc((1 + max_path_vertices), sizeof(int));

    if (!path || !has_visited) {
        printf("\nAllocation failure in logShortestPathsNoCycles.\n");
        return -1;
    }

    // initialize:
    for (int i = 0; i < num_vertices; ++i) {
        // mark vertices that do not exist as processed
        if (!graph->adj_list[i]) has_visited[i] = 1;
        // else has_processed[i] is already 0
    }

    // Find all paths:
    Stack *stack = initStack();
    setDataFunctionsStack(stack, NULL, destroyPairInt);

    // start is the 1st vertex in the path:
    pushStack(stack, initPairInt(start, 1));

    while (!isEmptyStack(stack)) {
        PairInt *cur_pair = topStack(stack);

        const int vertex_idx = cur_pair->first;  // 0-indexed
        // idx in arr path to insert the vertex into
        int vertex_path_idx = cur_pair->second;  // 1-indexed
        popStack(stack);

        if (vertex_path_idx < 0) {
        // bottom of recursion
            vertex_path_idx *= -1;
            has_visited[vertex_idx] = 0;
            continue;
        }

        // plant the bottom of local recursion in the stack
        pushStack(stack, initPairInt(vertex_idx, -1 * vertex_path_idx));

        // entering recursion first time:

        path[vertex_path_idx] = vertex_idx;
        has_visited[vertex_idx] = 1;

        // path of given length was constructed
        if (vertex_path_idx == max_path_vertices) {
            has_visited[vertex_idx] = 0;
            // if it failed to reach end with given number of
            // moves just continue
            if (vertex_idx != end) continue;

            // log the path from start to end of given length
            printf("Path:\n");
            for (int i = 1; i <= vertex_path_idx; ++i) {
                printf("%d ", path[i]);
            }
            printf("\n");
            continue;
        }


        // keep building all possible paths from current
        for (Node *cur = graph->adj_list[vertex_idx]->head;
             cur != NULL; cur = cur->next)
        {
            int new_vertex = ((Edge*) cur->data)->dest;
            if (has_visited[new_vertex]) continue;
            pushStack(stack, initPairInt(new_vertex, 1 + vertex_path_idx));
        }
    }

    destroyStack(stack);
    free(has_visited);
    free(path);
}


// does BFS and plays the game move by move
// returns min number of moves to reach end
// if end is unreachable returns INT_MAX
// returns -1 in case of error
int findSanjasShortestPath(const Graph *graph, const int start, const int end) {

    // number of moves in shortest Sanja's path
    int min_num_moves = INT_MAX;

    if (!graph || !graph->adj_list_length) return -1;

    // in case there are no outgoing edges from start:
    if (!graph->adj_list[start]
     || graph->adj_list[start]->length == 0) {
        // if end is unreachable return INF
        if (start != end) return INT_MAX;
        // already at end
        return 0;
    }

    // num of vertices to explore on the path of given lengths
    const int num_vertices = graph->adj_list_length;
    const int num_moves = 2;  // max num moves sanja can make at once

    // has_visited[i][L] = 0/1 i-th vertex which was reached from
    // vertex with L=(num_moves_to_make)%2 has/hasn't been visited
    short **has_visited = (short**) calloc(num_vertices, sizeof(short*));

    if (!has_visited) {
        printf("\nAllocation failure in findSanjasShortestPath.\n");
        return -1;
    }

    // initialize has_visited:
    for (int i = 0; i < num_vertices; ++i) {
        has_visited[i] = (short*) calloc(num_moves, sizeof(short));

        if (!has_visited[i]) {
            printf("\nAllocation failure in findSanjasShortestPath.\n");
            return -1;
        }

        // set 1 for all of the vertices that do not exist
        if (graph->adj_list[i]) continue;
        for (int j = 0; j < num_moves; ++j) {
            has_visited[i][j] = 1;
        }
    }

    Queue *queue = initQueue();
    setDataFunctionsQueue(queue, NULL, destroyPairInt);

    // queue contains pairs where first value is idx
    // of the vertex and second value is number of moves
    // it took to reach that vertex
    pushQueue(queue, initPairInt(start, 0));

    while (!isEmptyQueue(queue)) {
        PairInt* cur_pair = frontQueue(queue);
        const int vertex_idx = cur_pair->first;
        // num of moves it took to get here
        const int move_idx = cur_pair->second;
        popQueue(queue);

        // reached the end (this was shortest path)
        // (not for the 1st but for 1st time so that new
        // Sanja's move starts here)
        if (vertex_idx == end) {
            min_num_moves = move_idx;
            break;  // do not return cause it wouldnt free memory
            // return move_idx;
        }

        // odd moves: make 1 move
        // even moves: make 2 moves
        const int depth = (move_idx + 1) % num_moves == 1 ? 1 : 2;

        // add all new moves from here
        // queue's pair: (vertex_idx_to_add, num_moves_made)
        Queue *to_add = initQueue();
        setDataFunctionsQueue(to_add, NULL, destroyPairInt);

        pushQueue(to_add, initPairInt(vertex_idx, 0));
        while (!isEmptyQueue(to_add)) {
            cur_pair = frontQueue(to_add);
            const int cur_vertex = cur_pair->first;
            const int cur_depth = cur_pair->second;
            popQueue(to_add);

            if (cur_depth == depth) {
                // add to be processed in BFS if it hasn't been
                // already:
                if (!has_visited[cur_vertex][depth % num_moves]) {
                    pushQueue(queue, initPairInt(cur_vertex, 1 + move_idx));
                    has_visited[cur_vertex][depth % num_moves] = 1;
                }
                continue;
            }

            // add all of curs reachable unexplored neighbours
            for (Node *cur = graph->adj_list[cur_vertex]->head;
                 cur != NULL; cur = cur->next)
            {
                const int to_add_vertex = ((Edge*) cur->data)->dest;
                pushQueue(to_add, initPairInt(to_add_vertex, 1 + cur_depth));
            }
        }

        destroyQueue(to_add);
    }  // endwhile

    destroyQueue(queue);
    // free has_visited:
    for (int i = 0; i < num_vertices; ++i) {
        free(has_visited[i]);
    }
    free(has_visited);

    // no path was found
    return min_num_moves;
}


// plays the game as Sanja from start to end
// for given number of moves and whenever it reaches
// the end it logs the path to get there
void logSanjasShortestPaths(const Graph *graph, const int start, const int end,
                            const int path_num_moves) {
    if (!graph || !graph->adj_list_length) return;
    if (path_num_moves == INT_MAX || path_num_moves < 0) return;

    // in case there are no outgoing edges from start:
    if (!graph->adj_list[start]
     || graph->adj_list[start]->length == 0) {
        // if end is unreachable return INF
        if (start != end) {
            printf("Sanja cannot reach %d from %d\n", end, start);
        } else {
            printf("Path:\n%d\n", start);
        }
        return;
    }


    // num of vertices to explore on the path of given lengths
    const int num_vertices = graph->adj_list_length;
    const int num_moves = 2;  // max num moves sanja can make at once
    // self + num of vertices visited with path_num_moves of moves
    const int max_path_vertices = 1 + path_num_moves + path_num_moves / 2;

    // has_visited[i][L] = 0/1 i-th vertex out of which there
    // are L in between moves, has/hasn't been visited
    short **has_visited = (short**) calloc(num_vertices, sizeof(short*));
    // array of vertices forming single path which is being processed
    // 1-indexed (so the inverse sign flagging is possible):
    int *path = (int*) calloc((1 + max_path_vertices), sizeof(int));

    if (!path || !has_visited) {
        printf("\nAllocation failure in findSanjasShortestPath.\n");
        return -1;
    }

    // initialize has_visited:
    for (int i = 0; i < num_vertices; ++i) {
        has_visited[i] = (short*) calloc(num_moves, sizeof(short));

        if (!has_visited[i]) {
            printf("\nAllocation failure in findSanjasShortestPath.\n");
            return -1;
        }

        // set 1 for all of the vertices that do not exist
        if (graph->adj_list[i]) continue;
        for (int j = 0; j < num_moves; ++j) {
            has_visited[i][j] = 1;
        }
    }

    // Find all paths:
    Stack *stack = initStack();
    setDataFunctionsStack(stack, NULL, destroyPairInt);

    // start is the 1st vertex in the path:
    pushStack(stack, initPairInt(start, 1));

    while (!isEmptyStack(stack)) {
        PairInt *cur_pair = topStack(stack);
        const int vertex_idx = cur_pair->first;  // 0-indexed
        // idx in arr path to insert the vertex into
        int vertex_path_idx = cur_pair->second;  // 1-indexed
        int cur_move_idx = calcSanjasMoveIdx(vertex_path_idx);
        popStack(stack);

        if (vertex_path_idx < 0) {
        // bottom of recursion
            vertex_path_idx *= -1;  // make it positive again
            cur_move_idx = calcSanjasMoveIdx(vertex_path_idx);
            has_visited[vertex_idx][cur_move_idx % num_moves] = 0;
            continue;
        }

        // plant the bottom of local recursion in the stack
        pushStack(stack, initPairInt(vertex_idx, -1 * vertex_path_idx));

        // entering recursion first time:

        path[vertex_path_idx] = vertex_idx;
        // recheck abstraction from 2 to num_moves here:
        if (vertex_path_idx % (1 + num_moves) != 0) {  // not an inbetween move
            has_visited[vertex_idx][cur_move_idx % num_moves] = 1;
        }

        // path of given length was constructed
        if (vertex_path_idx == max_path_vertices) {
            has_visited[vertex_idx][cur_move_idx % num_moves] = 0;
            // if it failed to reach end with given number of
            // moves just continue
            if (vertex_idx != end) continue;

            // log the path from start to end of given length
            printf("Path:\n");
            int in_between_move_idx = 0;
            printf("%d ", path[1]);  // log start
            for (int i = 2; i <= vertex_path_idx; ++i) {
                if (in_between_move_idx == 1) printf("( ");

                printf("%d ", path[i]);

                if (in_between_move_idx == num_moves) {
                    printf(") ");
                    in_between_move_idx = 0;
                } else  ++in_between_move_idx;
            }
            printf("\n");
            continue;
        }

        // keep building all possible paths from current
        for (Node *cur = graph->adj_list[vertex_idx]->head;
             cur != NULL; cur = cur->next)
        {
            const int new_vertex = ((Edge*) cur->data)->dest;
            const int new_move_idx = calcSanjasMoveIdx(1 + vertex_path_idx);
            if (has_visited[new_vertex][new_move_idx % num_moves]) continue;
            pushStack(stack, initPairInt(new_vertex, 1 + vertex_path_idx));
        }
    }

    destroyStack(stack);
    // free has_visited:
    for (int i = 0; i < num_vertices; ++i) {
        free(has_visited[i]);
    }
    free(has_visited);
    free(path);

    return;
}


// returns in which move given vertex would have been
// visited
int calcSanjasMoveIdx(const int vertex_path_idx) {
    return 2 * (vertex_path_idx / 3) + vertex_path_idx % 3 - 1;
}


// ====================================
// Testing:


void testQueue() {
    int cur_idx = 0;
    int start = 17;

    Queue *queue = initQueue();
    setDataFunctionsQueue(queue, NULL, destroyPairInt);

    pushQueue(queue, initPairInt(1, cur_idx));
    popQueue(queue);
    pushQueue(queue, initPairInt(2, cur_idx));
    // // popQueue(queue);
    // pushQueue(queue, initPairInt(3, cur_idx));
    // popQueue(queue);
    // pushQueue(queue, initPairInt(4, cur_idx));
    // popQueue(queue);
    // // popQueue(queue);
    // pushQueue(queue, initPairInt(5, cur_idx));
    // popQueue(queue);

    while (!isEmptyQueue(queue)) {
        PairInt* cur_pair = frontQueue(queue);
        printf("%d %d\n", cur_pair->first, cur_pair->second);
        popQueue(queue);
    }

    destroyQueue(queue);
    printf("compiles\n");
}

void testStack() {
    int cur_idx = 0;
    int start = 17;

    Stack *stack = initStack();
    setDataFunctionsStack(stack, NULL, destroyPairInt);

    pushStack(stack, initPairInt(1, cur_idx));
    popStack(stack);
    pushStack(stack, initPairInt(2, cur_idx));
    popStack(stack);
    pushStack(stack, initPairInt(3, cur_idx));
    pushStack(stack, initPairInt(4, cur_idx));
    popStack(stack);
    popStack(stack);
    pushStack(stack, initPairInt(5, cur_idx));
    popStack(stack);

    while (!isEmptyStack(stack)) {
        PairInt* cur_pair = topStack(stack);
        printf("%d %d\n", cur_pair->first, cur_pair->second);
        popStack(stack);
    }

    destroyStack(stack);
    printf("compiles\n");
}

void tmpTest() {
    LinkedList *list = initLinkedList();
    appendValue(list, 6);
    displayIntLinkedList(list);
    removeValues(list, 6);
    displayIntLinkedList(list);
}

void testLinkedList() {
    int x, y, z;

    scanf("%d %d %d", &x, &y, &z);

    Edge *e1 = calloc(1, sizeof(Edge));
    e1->dest = x;

    Edge *e2 = calloc(1, sizeof(Edge));
    e2->dest = y;

    Edge *e3 = calloc(1, sizeof(Edge));
    e3->dest = z;

    LinkedList *list = initLinkedList();
    setDataFunctionsLL(list, areEqualEdges, destroyEdge);

    appendValue(list, e1);
    appendValue(list, e2);
    appendValue(list, e3);

    displayEdgeLinkedList(list);
    
    Edge *e4 = calloc(1, sizeof(Edge));
    e4->dest = 3;
    Node *found = findValue(list, e4);
    printf("Found: %d;\n", ((Edge*)found->data)->dest);

    found = deleteLinkedListNode(list, found);
    displayEdgeLinkedList(list);


    e4->dest = 2;
    found = findValue(list, e4);
    found = deleteLinkedListNode(list, found);
    
    Edge *tmp = calloc(1, sizeof(Edge));
    tmp->dest = z;
    appendValue(list, tmp);
    displayEdgeLinkedList(list);
    e4->dest = 1;
    found = findValue(list, e4);
    found = deleteLinkedListNode(list, found);
    
    
    
    displayEdgeLinkedList(list);
    // popFront(list);

    // popBack(list);
    // for (Node *cur = list->head; cur != NULL; cur = cur->next) {
    //     printf("%d\n", ((Edge*)cur->data)->dest);
    // }
    free(e4);
    destroyLinkedList(list);
}


void displayEdgeLinkedList(const LinkedList *list) {
    printf("List:\n");
    for (Node *cur = list->head; cur != NULL; cur = cur->next) {
        printf("%d\n", ((Edge*)cur->data)->dest);
    }
}

void displayPairIntLinkedList(const LinkedList *list) {
    printf("List:\n");
    for (Node *cur = list->head; cur != NULL; cur = cur->next) {
        PairInt *pair = ((PairInt*)cur->data);
        printf("(%d %d)\n", pair->first, pair->second);
    }
}

void displayIntLinkedList(const LinkedList *list) {
    printf("List ");
    printf("length: %d\n", list->length);
    for (Node *cur = list->head; cur != NULL; cur = cur->next) {
        printf("%d\n", cur->data);
    }
}


/*
	Tests:
	
8
6
8
0 1
2 0
1 3
1 5
2 5
5 4
4 2
3 4
0
5
21

The winner is: Maja!

Maja's number of moves: 2

Sanja's number of moves: 5

Maja's paths:
Path:
0 1 5

Sanja's paths:
Path:
0 1 ( 5 4 ) 2 ( 0 1 ) 5
Path:
0 1 ( 3 4 ) 2 ( 0 1 ) 5

// Test case 2:
8
6
7
0 1
2 0
1 3
2 3
4 2
3 5
5 4
0
5
21

The winner is: Sanja!

Maja's number of moves: 3

Sanja's number of moves: 2

Maja's paths:
Path:
0 1 3 5

Sanja's paths:
Path:
0 1 ( 3 5 )


*/
