#include "campus_graph.h"

void g_init(Graph *g) { memset(g, 0, sizeof *g); }

int g_add_vertex(Graph *g, const char *name) {
    if (g->n >= MAX_V || g_find(g, name) >= 0) return -1;
    strncpy(g->name[g->n], name, NAME_LEN - 1);
    g->name[g->n][NAME_LEN - 1] = '\0';
    return g->n++;
}

int g_find(const Graph *g, const char *name) {
    for (int i = 0; i < g->n; i++)
        if (strcmp(g->name[i], name) == 0) return i;
    return -1;
}

static int has_edge(const Graph *g, int u, int v) {
    for (AdjNode *a = g->adj[u]; a; a = a->next)
        if (a->v == v) return 1;
    return 0;
}

static void push_adj(Graph *g, int u, int v) {
    /* append at tail so neighbours keep insertion order (stable output) */
    AdjNode *n = malloc(sizeof *n);
    n->v = v; n->next = NULL;
    AdjNode **pp = &g->adj[u];
    while (*pp) pp = &(*pp)->next;
    *pp = n;
}

int g_add_edge(Graph *g, int u, int v) {
    if (u < 0 || v < 0 || u >= g->n || v >= g->n || u == v) return 0;
    if (has_edge(g, u, v)) return 0;
    push_adj(g, u, v);
    push_adj(g, v, u);
    g->edges++;
    return 1;
}

void g_print_list(const Graph *g) {
    for (int i = 0; i < g->n; i++) {
        printf("  %2d %-16s:", i, g->name[i]);
        for (AdjNode *a = g->adj[i]; a; a = a->next) printf(" %d", a->v);
        printf("\n");
    }
}

void g_print_matrix(const Graph *g) {
    printf("     ");
    for (int j = 0; j < g->n; j++) printf("%3d", j);
    printf("\n");
    for (int i = 0; i < g->n; i++) {
        printf("  %2d ", i);
        for (int j = 0; j < g->n; j++) printf("%3d", has_edge(g, i, j));
        printf("\n");
    }
}

/* ================= BFS ================= */

int g_bfs_path(const Graph *g, int src, int dst, int *path) {
    int visited[MAX_V] = {0}, parent[MAX_V];
    for (int i = 0; i < MAX_V; i++) parent[i] = -1;

    IntQueue *q = iq_create(MAX_V);
    visited[src] = 1; iq_enqueue(q, src);
    while (!iq_empty(q)) {
        int u = iq_dequeue(q);
        if (u == dst) break;
        for (AdjNode *a = g->adj[u]; a; a = a->next)
            if (!visited[a->v]) {
                visited[a->v] = 1;
                parent[a->v] = u;
                iq_enqueue(q, a->v);
            }
    }
    iq_free(q);
    if (!visited[dst]) return 0;

    /* walk parents back from dst, then reverse */
    int len = 0;
    for (int v = dst; v != -1; v = parent[v]) path[len++] = v;
    for (int i = 0; i < len / 2; i++) {
        int t = path[i]; path[i] = path[len - 1 - i]; path[len - 1 - i] = t;
    }
    return len;
}

void g_bfs_order(const Graph *g, int src) {
    int visited[MAX_V] = {0};
    IntQueue *q = iq_create(MAX_V);
    visited[src] = 1; iq_enqueue(q, src);
    printf("  BFS:");
    while (!iq_empty(q)) {
        int u = iq_dequeue(q);
        printf(" %s", g->name[u]);
        for (AdjNode *a = g->adj[u]; a; a = a->next)
            if (!visited[a->v]) { visited[a->v] = 1; iq_enqueue(q, a->v); }
        if (!iq_empty(q)) printf(" ->");
    }
    printf("\n");
    iq_free(q);
}

/* ================= DFS (recursive) ================= */

static int dfs_rec(const Graph *g, int u, int dst, int *visited,
                   int *path, int *len) {
    visited[u] = 1;
    path[(*len)++] = u;
    if (u == dst) return 1;
    for (AdjNode *a = g->adj[u]; a; a = a->next)
        if (!visited[a->v] && dfs_rec(g, a->v, dst, visited, path, len))
            return 1;
    (*len)--;                          /* backtrack */
    return 0;
}

int g_dfs_path(const Graph *g, int src, int dst, int *path, int *len) {
    int visited[MAX_V] = {0};
    *len = 0;
    return dfs_rec(g, src, dst, visited, path, len);
}

static void dfs_print(const Graph *g, int u, int *visited, int *first) {
    visited[u] = 1;
    printf("%s%s", *first ? " " : " -> ", g->name[u]);
    *first = 0;
    for (AdjNode *a = g->adj[u]; a; a = a->next)
        if (!visited[a->v]) dfs_print(g, a->v, visited, first);
}

void g_dfs_order(const Graph *g, int src) {
    int visited[MAX_V] = {0}, first = 1;
    printf("  DFS:");
    dfs_print(g, src, visited, &first);
    printf("\n");
}

int g_components(const Graph *g) {
    int visited[MAX_V] = {0}, comps = 0, path[MAX_V], len;
    for (int i = 0; i < g->n; i++)
        if (!visited[i]) {
            comps++;
            /* reuse DFS with an impossible target to flood one component */
            int vis2[MAX_V] = {0};
            len = 0;
            dfs_rec(g, i, -1, vis2, path, &len);
            for (int j = 0; j < g->n; j++) if (vis2[j]) visited[j] = 1;
        }
    return comps;
}

void g_free(Graph *g) {
    for (int i = 0; i < g->n; i++) {
        AdjNode *a = g->adj[i];
        while (a) { AdjNode *n = a->next; free(a); a = n; }
        g->adj[i] = NULL;
    }
}

/* ================= Menu ================= */

static int ask_vertex(const Graph *g, const char *prompt) {
    for (int i = 0; i < g->n; i++) printf("  %2d %s\n", i, g->name[i]);
    int v = read_int(prompt);
    return (v >= 0 && v < g->n) ? v : -1;
}

void graph_menu(Graph *g) {
    for (;;) {
        print_line();
        printf(" CAMPUS NAVIGATOR  (Graph: %d places, %d walkways, %d component(s))\n",
               g->n, g->edges, g_components(g));
        print_line();
        printf(" 1. Shortest route (BFS)     2. Does a route exist? (DFS)\n");
        printf(" 3. BFS order from a place   4. DFS order from a place\n");
        printf(" 5. Adjacency list           6. Adjacency matrix\n");
        printf(" 7. Add place                8. Add walkway\n");
        printf(" 0. Back\n");
        int ch = read_int("Choice: ");
        if (ch == 0) return;
        int path[MAX_V], len;
        switch (ch) {
        case 1: case 2: {
            int s = ask_vertex(g, "From: "), d = ask_vertex(g, "To: ");
            if (s < 0 || d < 0) { printf("  Invalid place.\n"); break; }
            int found = (ch == 1) ? (len = g_bfs_path(g, s, d, path)) > 0
                                  : g_dfs_path(g, s, d, path, &len);
            if (!found) { printf("  No route.\n"); break; }
            printf("  %s route (%d stop(s)):\n  ", ch == 1 ? "Shortest" : "DFS", len - 1);
            for (int i = 0; i < len; i++)
                printf("%s%s", g->name[path[i]], i + 1 < len ? " -> " : "\n");
            break;
        }
        case 3: case 4: {
            int s = ask_vertex(g, "Start: ");
            if (s < 0) { printf("  Invalid place.\n"); break; }
            if (ch == 3) g_bfs_order(g, s); else g_dfs_order(g, s);
            break;
        }
        case 5: g_print_list(g); break;
        case 6: g_print_matrix(g); break;
        case 7: {
            char nm[NAME_LEN];
            read_line("Place name: ", nm, NAME_LEN);
            printf(g_add_vertex(g, nm) >= 0 ? "  Added.\n" : "  Could not add.\n");
            break;
        }
        case 8: {
            int u = ask_vertex(g, "First place: "), v = ask_vertex(g, "Second place: ");
            printf(g_add_edge(g, u, v) ? "  Walkway added.\n" : "  Could not add.\n");
            break;
        }
        default: printf("  Invalid choice.\n");
        }
        pause_screen();
    }
}
