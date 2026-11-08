/*
 * CampusHub - A Data Structures & Algorithms mini project (CSC2101)
 *
 *   1. Student Records  -> AVL tree (BST + rotations), stack, queue
 *   2. Library          -> Hash table (4 collision strategies)
 *   3. Campus Navigator -> Graph (adjacency list/matrix), BFS, DFS
 *   4. Help Desk        -> Max-heap priority queue
 *   5. Algorithm Lab    -> Merge/Quick/Heap sort, binary search,
 *                          max-min (D&C), Tower of Hanoi, benchmark
 *
 * Usage:  ./campushub            interactive menu
 *         ./campushub --bench    print sorting benchmark as CSV
 */
#include "seed_data.h"
#include "algo_lab.h"

static ProbeMode choose_mode(void) {
    printf("\nChoose library collision technique:\n");
    printf(" 1. Linear probing  2. Quadratic probing  3. Double hashing  4. Separate chaining\n");
    int m = read_int("Choice [1-4]: ");
    return (m >= 1 && m <= 4) ? (ProbeMode)m : LINEAR;
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--bench") == 0) {
        run_benchmark(stdout);
        return 0;
    }

    AVLNode  *students = seed_students(NULL);
    HashTable library;
    Graph     campus;
    MaxHeap   desk;

    printf("============================================================\n");
    printf("        CampusHub  -  DSA Mini Project (CSC2101)\n");
    printf("============================================================\n");

    ht_init(&library, choose_mode());
    seed_library(&library);
    g_init(&campus);
    seed_campus(&campus);
    heap_init(&desk);
    seed_helpdesk(&desk);

    for (;;) {
        print_line();
        printf(" MAIN MENU\n");
        print_line();
        printf(" 1. Student Records   (AVL Tree)\n");
        printf(" 2. Library           (Hash Table)\n");
        printf(" 3. Campus Navigator  (Graph, BFS/DFS)\n");
        printf(" 4. Help Desk         (Max-Heap)\n");
        printf(" 5. Algorithm Lab     (Sorting, Searching, Recursion)\n");
        printf(" 0. Exit\n");
        switch (read_int("Choice: ")) {
        case 1: student_menu(&students); break;
        case 2: library_menu(&library);  break;
        case 3: graph_menu(&campus);     break;
        case 4: helpdesk_menu(&desk);    break;
        case 5: algo_menu(students);     break;
        case 0:
            avl_free(students);
            ht_free(&library);
            g_free(&campus);
            printf("Goodbye!\n");
            return 0;
        default: printf("  Invalid choice.\n");
        }
    }
}
