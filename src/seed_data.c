#include "seed_data.h"

AVLNode *seed_students(AVLNode *root) {
    /* Rolls inserted in ascending order on purpose: a plain BST would
     * degrade into a linked list; the AVL tree stays balanced. */
    Student s[] = {
        {101, "Aarav Sharma", 8.45f}, {102, "Diya Patel", 9.12f},
        {103, "Kabir Singh", 7.80f},  {104, "Ananya Gupta", 9.40f},
        {105, "Rohan Mehta", 6.95f},  {106, "Ishita Verma", 8.88f},
        {107, "Vihaan Rao", 7.25f},   {108, "Meera Nair", 9.05f},
        {109, "Arjun Joshi", 8.10f},  {110, "Sara Khan", 8.67f},
    };
    int ok;
    for (size_t i = 0; i < sizeof s / sizeof s[0]; i++)
        root = avl_insert(root, s[i], &ok);
    return root;
}

void seed_library(HashTable *ht) {
    Book b[] = {
        {1201, "Data Structures Using C", "Reema Thareja", 1},
        {1232, "Introduction to Algorithms", "Cormen et al.", 1},
        {1263, "The C Programming Language", "Kernighan", 1},
        {2045, "Algorithm Design", "Kleinberg", 1},
        {3110, "Fundamentals of DS in C", "Horowitz Sahni", 1},
        {4021, "Database System Concepts", "Silberschatz", 1},
        {5007, "Discrete Mathematics", "Rosen", 1},
    };
    int p;
    for (size_t i = 0; i < sizeof b / sizeof b[0]; i++) ht_insert(ht, b[i], &p);
}

void seed_campus(Graph *g) {
    const char *places[] = {
        "Main Gate", "Admin Block", "Library", "AB-1", "AB-2",
        "Canteen", "Hostel", "Sports Complex", "Auditorium", "Parking",
    };
    for (size_t i = 0; i < sizeof places / sizeof places[0]; i++)
        g_add_vertex(g, places[i]);
    int e[][2] = {
        {0, 1}, {0, 9}, {1, 2}, {1, 3}, {2, 3}, {3, 4}, {4, 5},
        {5, 6}, {6, 7}, {2, 8}, {8, 4}, {9, 7},
    };
    for (size_t i = 0; i < sizeof e / sizeof e[0]; i++)
        g_add_edge(g, e[i][0], e[i][1]);
}

void seed_helpdesk(MaxHeap *h) {
    heap_push(h, 2, "Wi-Fi slow in AB-2");
    heap_push(h, 5, "Fire alarm fault in Hostel");
    heap_push(h, 3, "Projector broken in AB-1 101");
    heap_push(h, 4, "Water leakage in Library");
    heap_push(h, 1, "Request extra chairs");
}
