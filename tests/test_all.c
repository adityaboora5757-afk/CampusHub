/* Automated tests for every CampusHub module.  Run with:  make test */
#include "seed_data.h"
#include "algo_lab.h"

static int passed = 0, failed = 0;

#define CHECK(cond, msg) do {                                   \
        if (cond) { passed++; printf("  [PASS] %s\n", msg); }    \
        else      { failed++; printf("  [FAIL] %s\n", msg); }    \
    } while (0)

static void test_avl(void) {
    printf("\nAVL tree (student records)\n");
    AVLNode *r = NULL;
    int ok;
    avl_rotation_count = 0;
    /* 1000 ascending keys: worst case for a plain BST */
    for (int i = 1; i <= 1000; i++) {
        Student s = {i, "x", 7.0f};
        r = avl_insert(r, s, &ok);
    }
    CHECK(avl_count(r) == 1000, "1000 students inserted");
    CHECK(avl_is_balanced(r), "AVL + BST property holds after 1000 sorted inserts");
    CHECK(avl_height(r) <= 15, "height <= 1.44 log2(n) (got <= 15, plain BST would be 1000)");
    CHECK(avl_rotation_count > 0, "rotations were performed");

    Student dup = {500, "dup", 1.0f};
    r = avl_insert(r, dup, &ok);
    CHECK(!ok, "duplicate roll rejected");

    CHECK(avl_search_rec(r, 777) && avl_search_iter(r, 777), "search finds 777 (rec + iter)");
    CHECK(!avl_search_rec(r, 5000) && !avl_search_iter(r, 5000), "search misses 5000 (rec + iter)");

    for (int i = 1; i <= 1000; i += 2) r = avl_delete(r, i, &ok, NULL);
    CHECK(avl_count(r) == 500, "500 odd rolls deleted");
    CHECK(avl_is_balanced(r), "tree still balanced after deletions");
    CHECK(!avl_search_iter(r, 501) && avl_search_iter(r, 502), "deleted key gone, kept key present");

    Student arr[500];
    int n = avl_to_array(r, arr, 0);
    int sorted = 1;
    for (int i = 1; i < n; i++) if (arr[i - 1].roll >= arr[i].roll) sorted = 0;
    CHECK(n == 500 && sorted, "inorder traversal is sorted");
    avl_free(r);
}

static void test_hash(void) {
    printf("\nHash table (library)\n");
    for (int m = LINEAR; m <= CHAINING; m++) {
        HashTable ht;
        ht_init(&ht, (ProbeMode)m);
        seed_library(&ht);
        int p, good = 1;
        int ids[] = {1201, 1232, 1263, 2045, 3110, 4021, 5007};
        for (int i = 0; i < 7; i++) if (!ht_search(&ht, ids[i], &p)) good = 0;
        char msg[96];
        sprintf(msg, "%s: all 7 books found", ht_mode_name((ProbeMode)m));
        CHECK(good, msg);

        ht_delete(&ht, 1232);                  /* 1201,1232,1263 collide at 23 */
        sprintf(msg, "%s: search past a deleted slot still works", ht_mode_name((ProbeMode)m));
        CHECK(!ht_search(&ht, 1232, &p) && ht_search(&ht, 1263, &p), msg);

        Book d = {1201, "dup", "dup", 1};
        sprintf(msg, "%s: duplicate ID rejected", ht_mode_name((ProbeMode)m));
        CHECK(!ht_insert(&ht, d, &p), msg);
        ht_free(&ht);
    }
}

static void test_graph(void) {
    printf("\nGraph (campus navigator)\n");
    Graph g; g_init(&g); seed_campus(&g);
    int path[MAX_V], len;
    len = g_bfs_path(&g, g_find(&g, "Main Gate"), g_find(&g, "Canteen"), path);
    CHECK(len == 5, "BFS: Main Gate -> Canteen takes 4 hops");
    len = g_bfs_path(&g, 0, 0, path);
    CHECK(len == 1, "BFS: path to itself has length 1");
    CHECK(g_dfs_path(&g, 0, g_find(&g, "Sports Complex"), path, &len) && path[len - 1] == 7,
          "DFS finds a route to Sports Complex");
    int lone = g_add_vertex(&g, "Observatory");
    CHECK(g_bfs_path(&g, 0, lone, path) == 0, "BFS reports no route to an isolated place");
    CHECK(!g_dfs_path(&g, 0, lone, path, &len), "DFS reports no route to an isolated place");
    CHECK(g_components(&g) == 2, "2 connected components after adding isolated place");
    CHECK(!g_add_edge(&g, 0, 1), "duplicate edge rejected");
    g_free(&g);
}

static void test_heap(void) {
    printf("\nMax-heap (help desk)\n");
    MaxHeap h; heap_init(&h); seed_helpdesk(&h);
    CHECK(heap_is_valid(&h), "heap property holds after inserts");
    Ticket t; int prev = 6, order_ok = 1;
    while (heap_pop(&h, &t)) { if (t.priority > prev) order_ok = 0; prev = t.priority; }
    CHECK(order_ok, "tickets served in non-increasing priority");
    heap_push(&h, 3, "first"); heap_push(&h, 3, "second");
    heap_pop(&h, &t);
    CHECK(strcmp(t.text, "first") == 0, "equal priority served first-come-first-served");
    CHECK(!heap_push(&h, 9, "bad"), "invalid priority rejected");
}

static void test_algorithms(void) {
    printf("\nAlgorithm lab\n");
    enum { N = 20000 };
    static int src[N], a[N];
    srand(7);
    for (int i = 0; i < N; i++) src[i] = rand() % 100000;
    void (*fns[])(int *, int) = {merge_sort, quick_sort, heap_sort, insertion_sort};
    const char *names[] = {"merge sort", "quick sort", "heap sort", "insertion sort"};
    for (int k = 0; k < 4; k++) {
        memcpy(a, src, sizeof a);
        fns[k](a, N);
        char msg[64]; sprintf(msg, "%s sorts 20000 random ints", names[k]);
        CHECK(is_sorted(a, N), msg);
    }
    for (int i = 0; i < N; i++) a[i] = i;           /* already sorted input */
    quick_sort(a, N);
    CHECK(is_sorted(a, N) && sort_comparisons < 2L * N * 20,
          "quick sort stays O(n log n) on sorted input (median-of-three)");

    int s[] = {2, 5, 9, 14, 21, 30, 42};
    CHECK(binary_search_iter(s, 7, 21) == 4 && binary_search_rec(s, 0, 6, 21) == 4,
          "binary search finds 21 at index 4");
    CHECK(binary_search_iter(s, 7, 8) == -1 && binary_search_rec(s, 0, 6, 8) == -1,
          "binary search misses 8");

    int v[] = {7, 2, 19, -3, 11, 4, 8, 15};
    int mx, mn, c = max_min_dc(v, 0, 7, &mx, &mn);
    CHECK(mx == 19 && mn == -3, "max-min finds 19 and -3");
    CHECK(c == 10, "max-min uses 3n/2-2 = 10 comparisons for n=8");

    CHECK(hanoi(10, 'A', 'C', 'B', 0) == 1023, "Hanoi(10) takes 1023 moves");

    Student st[] = {{1, "a", 7.5f}, {2, "b", 9.1f}, {3, "c", 7.5f}, {4, "d", 8.0f}};
    merge_sort_students(st, 4);
    CHECK(st[0].roll == 2 && st[1].roll == 4 && st[2].roll == 1 && st[3].roll == 3,
          "student ranking by CGPA is correct and stable");
}

int main(void) {
    printf("CampusHub test suite\n====================");
    test_avl();
    test_hash();
    test_graph();
    test_heap();
    test_algorithms();
    printf("\nResult: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
