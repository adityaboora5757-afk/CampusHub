# CampusHub: a DSA Mini Project in C

CampusHub is a menu-driven campus management system for **CSC2101 Data Structures & Algorithms** (AY 2026-27). Each part of the campus runs on a different data structure from the syllabus, so one program covers the course from trees to divide and conquer.

| Module | What it does | Data structures / algorithms | Syllabus classes |
|---|---|---|---|
| Student Records | Add, delete, search and list students; undo a delete | AVL tree (BST + LL/RR/LR/RL rotations), recursive and non-recursive search and traversals, stack, queue | 3-4, 7-15 |
| Library | Add, search, issue, return and remove books by ID | Hash table with linear probing, quadratic probing, double hashing, or separate chaining | 29-31 |
| Campus Navigator | Shortest walking route and route existence between buildings | Graph (adjacency list and matrix), BFS, DFS, connected components | 23-28 |
| Help Desk | Serve complaints by urgency | Binary max-heap priority queue, build-heap | 21-22 |
| Algorithm Lab | Rank students, sort, search, Tower of Hanoi, benchmark | Merge, quick, heap and insertion sort; binary search; max-min (D&C); recursion; complexity analysis | 5-6, 22, 33-45 |

## Build and run

You need `gcc` and `make`. On Windows, use MinGW or WSL.

```bash
make          # build ./campushub
make run      # start the interactive menu
make test     # run the 45 automated tests
make bench    # sorting benchmark -> docs/benchmark.csv
make clean
```

When CampusHub starts, it asks which collision technique the library should use. Sample students, books, campus buildings and help-desk tickets load automatically, so you can try every feature straight away.

## Project structure

```
CampusHub/
├── include/            header files (one per module)
├── src/
│   ├── main.c          main menu
│   ├── common.c        input helpers, integer queue, pointer stack
│   ├── student_avl.c   Module 1: AVL tree
│   ├── library_hash.c  Module 2: hash table
│   ├── campus_graph.c  Module 3: graph, BFS, DFS
│   ├── helpdesk_heap.c Module 4: max-heap
│   ├── algo_lab.c      Module 5: sorting, searching, recursion
│   └── seed_data.c     sample data
├── tests/test_all.c    automated test suite
└── docs/               benchmark, sample run, test results
```

## Highlights

- **No degenerate trees.** The sample students are inserted in ascending roll order, which would turn a plain BST into a linked list. The AVL tree stays at height 4 for 10 students, and at most 15 for 1,000.
- **Correct deletion in open addressing.** Deleted slots become tombstones, so a search can still find keys that were placed after a collision.
- **Fair help desk.** Tickets with equal priority are served first-come, first-served.
- **Safe quick sort.** A median-of-three pivot and recursion on the smaller part keep quick sort at O(n log n) on already-sorted input, with O(log n) stack depth.
- **Measured, not just claimed.** The benchmark counts comparisons and times every sort on 1,000 to 100,000 random integers.

## Complexity summary

| Operation | Structure | Time |
|---|---|---|
| Insert / delete / search student | AVL tree | O(log n) |
| Add / search / remove book | Hash table | O(1) average, O(n) worst |
| Shortest route | BFS on adjacency list | O(V + E) |
| Route exists | DFS | O(V + E) |
| Raise / serve ticket | Max-heap | O(log n) |
| Merge / heap sort | Array | O(n log n) |
| Quick sort | Array | O(n log n) average |
| Binary search | Sorted array | O(log n) |
| Max-min (D&C) | Array | 3n/2 - 2 comparisons |
| Tower of Hanoi | Recursion | 2^n - 1 moves |

## License

MIT. See [LICENSE](LICENSE).
