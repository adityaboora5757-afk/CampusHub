#ifndef ALGO_LAB_H
#define ALGO_LAB_H

#include "common.h"
#include "student_avl.h"

/* Module 5: Algorithm Lab. Sorting, searching and recursion.
 * Covers syllabus classes 5-6 (iterative vs recursive, Tower of Hanoi),
 * 22 (heap sort), 33-41 (performance analysis), 42-45 (divide and
 * conquer: binary search, max-min, merge sort, quick sort). */

/* Sorting on int arrays (used for benchmarking) */
void merge_sort(int *a, int n);
void quick_sort(int *a, int n);
void heap_sort(int *a, int n);
void insertion_sort(int *a, int n);    /* O(n^2) baseline for comparison */

/* Sorting students by CGPA (descending) with merge sort - stable */
void merge_sort_students(Student *a, int n);

/* Searching */
int binary_search_iter(const int *a, int n, int key);
int binary_search_rec(const int *a, int lo, int hi, int key);

/* Divide and conquer max-min: returns number of comparisons made */
int max_min_dc(const int *a, int lo, int hi, int *mx, int *mn);

/* Recursion */
long hanoi(int n, char from, char to, char via, int print);

/* Instrumentation: comparisons made by the last sort call */
extern long sort_comparisons;

int  is_sorted(const int *a, int n);
void run_benchmark(FILE *out);         /* timing table for the report */

void algo_menu(AVLNode *students);

#endif
