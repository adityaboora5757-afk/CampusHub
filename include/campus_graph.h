#ifndef CAMPUS_GRAPH_H
#define CAMPUS_GRAPH_H

#include "common.h"

/* Module 3: Campus Navigator. Buildings are vertices, walkways are
 * undirected edges. Covers syllabus classes 23-28 (graph types,
 * adjacency matrix & list, BFS shortest path, DFS path existence). */

#define MAX_V 30

typedef struct AdjNode {
    int v;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    int      n;                     /* number of vertices */
    int      edges;
    char     name[MAX_V][NAME_LEN];
    AdjNode *adj[MAX_V];            /* adjacency list     */
} Graph;

void g_init(Graph *g);
int  g_add_vertex(Graph *g, const char *name);
int  g_add_edge(Graph *g, int u, int v);        /* undirected */
int  g_find(const Graph *g, const char *name);
void g_print_list(const Graph *g);
void g_print_matrix(const Graph *g);

/* BFS: fills path[] with the shortest (fewest hops) route, returns its
 * length in vertices, or 0 if unreachable. */
int  g_bfs_path(const Graph *g, int src, int dst, int *path);
void g_bfs_order(const Graph *g, int src);

/* DFS: returns 1 if dst reachable; fills path[] with the DFS route. */
int  g_dfs_path(const Graph *g, int src, int dst, int *path, int *len);
void g_dfs_order(const Graph *g, int src);

int  g_components(const Graph *g);              /* connected components */
void g_free(Graph *g);

void graph_menu(Graph *g);

#endif
