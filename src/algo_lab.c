#include "algo_lab.h"
#include <time.h>

long sort_comparisons = 0;

static void swap_i(int *x, int *y) { int t = *x; *x = *y; *y = t; }

int is_sorted(const int *a, int n) {
    for (int i = 1; i < n; i++) if (a[i - 1] > a[i]) return 0;
    return 1;
}

/* ================= Merge sort (divide & conquer) ================= */

static void merge(int *a, int *tmp, int lo, int mid, int hi) {
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi) {
        sort_comparisons++;
        tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    }
    while (i <= mid) tmp[k++] = a[i++];
    while (j <= hi)  tmp[k++] = a[j++];
    for (k = lo; k <= hi; k++) a[k] = tmp[k];
}

static void msort(int *a, int *tmp, int lo, int hi) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    msort(a, tmp, lo, mid);          /* divide   */
    msort(a, tmp, mid + 1, hi);
    merge(a, tmp, lo, mid, hi);      /* combine  */
}

void merge_sort(int *a, int n) {
    sort_comparisons = 0;
    if (n < 2) return;
    int *tmp = malloc(sizeof(int) * n);
    msort(a, tmp, 0, n - 1);
    free(tmp);
}

/* ================= Quick sort (Lomuto, median-of-three pivot) ========= */

static int partition(int *a, int lo, int hi) {
    int mid = lo + (hi - lo) / 2;
    /* median-of-three avoids O(n^2) on already-sorted input */
    if (a[mid] < a[lo]) swap_i(&a[mid], &a[lo]);
    if (a[hi]  < a[lo]) swap_i(&a[hi],  &a[lo]);
    if (a[mid] < a[hi]) swap_i(&a[mid], &a[hi]);   /* median now at hi */
    int pivot = a[hi], i = lo - 1;
    for (int j = lo; j < hi; j++) {
        sort_comparisons++;
        if (a[j] <= pivot) swap_i(&a[++i], &a[j]);
    }
    swap_i(&a[i + 1], &a[hi]);
    return i + 1;
}

static void qsort_rec(int *a, int lo, int hi) {
    while (lo < hi) {
        int p = partition(a, lo, hi);
        /* recurse on the smaller side -> O(log n) stack depth */
        if (p - lo < hi - p) { qsort_rec(a, lo, p - 1); lo = p + 1; }
        else                 { qsort_rec(a, p + 1, hi); hi = p - 1; }
    }
}

void quick_sort(int *a, int n) {
    sort_comparisons = 0;
    if (n > 1) qsort_rec(a, 0, n - 1);
}

/* ================= Heap sort ================= */

static void heapify(int *a, int n, int i) {
    for (;;) {
        int l = 2 * i + 1, r = l + 1, big = i;
        if (l < n) { sort_comparisons++; if (a[l] > a[big]) big = l; }
        if (r < n) { sort_comparisons++; if (a[r] > a[big]) big = r; }
        if (big == i) return;
        swap_i(&a[i], &a[big]);
        i = big;
    }
}

void heap_sort(int *a, int n) {
    sort_comparisons = 0;
    for (int i = n / 2 - 1; i >= 0; i--) heapify(a, n, i);   /* build max-heap */
    for (int end = n - 1; end > 0; end--) {
        swap_i(&a[0], &a[end]);                             /* move max to end */
        heapify(a, end, 0);
    }
}

/* ================= Insertion sort (baseline) ================= */

void insertion_sort(int *a, int n) {
    sort_comparisons = 0;
    for (int i = 1; i < n; i++) {
        int key = a[i], j = i - 1;
        while (j >= 0) {
            sort_comparisons++;
            if (a[j] <= key) break;
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

/* ================= Merge sort on students (by CGPA desc) ============= */

static void msort_st(Student *a, Student *tmp, int lo, int hi) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    msort_st(a, tmp, lo, mid);
    msort_st(a, tmp, mid + 1, hi);
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi)
        tmp[k++] = (a[i].cgpa >= a[j].cgpa) ? a[i++] : a[j++];  /* stable */
    while (i <= mid) tmp[k++] = a[i++];
    while (j <= hi)  tmp[k++] = a[j++];
    for (k = lo; k <= hi; k++) a[k] = tmp[k];
}

void merge_sort_students(Student *a, int n) {
    if (n < 2) return;
    Student *tmp = malloc(sizeof(Student) * n);
    msort_st(a, tmp, 0, n - 1);
    free(tmp);
}

/* ================= Binary search ================= */

int binary_search_iter(const int *a, int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] == key) return mid;
        if (a[mid] < key) lo = mid + 1; else hi = mid - 1;
    }
    return -1;
}

int binary_search_rec(const int *a, int lo, int hi, int key) {
    if (lo > hi) return -1;
    int mid = lo + (hi - lo) / 2;
    if (a[mid] == key) return mid;
    return a[mid] < key ? binary_search_rec(a, mid + 1, hi, key)
                        : binary_search_rec(a, lo, mid - 1, key);
}

/* ================= Max-Min (divide & conquer) =================
 * Uses about 3n/2 - 2 comparisons instead of 2n - 2 for the naive scan. */
int max_min_dc(const int *a, int lo, int hi, int *mx, int *mn) {
    if (lo == hi) { *mx = *mn = a[lo]; return 0; }
    if (hi == lo + 1) {
        if (a[lo] > a[hi]) { *mx = a[lo]; *mn = a[hi]; }
        else               { *mx = a[hi]; *mn = a[lo]; }
        return 1;
    }
    int mid = lo + (hi - lo) / 2, mx1, mn1, mx2, mn2;
    int c = max_min_dc(a, lo, mid, &mx1, &mn1)
          + max_min_dc(a, mid + 1, hi, &mx2, &mn2);
    *mx = mx1 > mx2 ? mx1 : mx2;
    *mn = mn1 < mn2 ? mn1 : mn2;
    return c + 2;
}

/* ================= Tower of Hanoi ================= */

long hanoi(int n, char from, char to, char via, int print) {
    if (n == 0) return 0;
    long moves = hanoi(n - 1, from, via, to, print);
    if (print) printf("  Move disk %d: %c -> %c\n", n, from, to);
    return moves + 1 + hanoi(n - 1, via, to, from, print);
}

/* ================= Benchmark ================= */

static double time_sort(void (*fn)(int *, int), const int *src, int n, long *cmp) {
    int *a = malloc(sizeof(int) * n);
    memcpy(a, src, sizeof(int) * n);
    clock_t t0 = clock();
    fn(a, n);
    double ms = 1000.0 * (clock() - t0) / CLOCKS_PER_SEC;
    *cmp = sort_comparisons;
    if (!is_sorted(a, n)) fprintf(stderr, "SORT FAILED\n");
    free(a);
    return ms;
}

void run_benchmark(FILE *out) {
    int sizes[] = {1000, 5000, 10000, 50000, 100000};
    int ns = sizeof sizes / sizeof sizes[0];
    struct { const char *name; void (*fn)(int *, int); int max_n; } algs[] = {
        {"Merge", merge_sort, 1 << 30},
        {"Quick", quick_sort, 1 << 30},
        {"Heap",  heap_sort,  1 << 30},
        {"Insertion", insertion_sort, 50000},
    };
    srand(42);
    fprintf(out, "n,algorithm,time_ms,comparisons\n");
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *src = malloc(sizeof(int) * n);
        for (int i = 0; i < n; i++) src[i] = rand() % 1000000;
        for (int k = 0; k < 4; k++) {
            if (n > algs[k].max_n) continue;
            long cmp;
            double ms = time_sort(algs[k].fn, src, n, &cmp);
            fprintf(out, "%d,%s,%.3f,%ld\n", n, algs[k].name, ms, cmp);
        }
        free(src);
    }
}

/* ================= Menu ================= */

static void print_arr(const int *a, int n) {
    printf("  [");
    for (int i = 0; i < n; i++) printf("%d%s", a[i], i + 1 < n ? ", " : "]\n");
}

void algo_menu(AVLNode *students) {
    for (;;) {
        print_line();
        printf(" ALGORITHM LAB  (sorting, searching, recursion)\n");
        print_line();
        printf(" 1. Rank students by CGPA (merge sort)\n");
        printf(" 2. Sort your own numbers (merge / quick / heap)\n");
        printf(" 3. Binary search a roll number (iterative + recursive)\n");
        printf(" 4. Max & min of CGPAs (divide and conquer)\n");
        printf(" 5. Tower of Hanoi\n");
        printf(" 6. Run sorting benchmark\n");
        printf(" 0. Back\n");
        int ch = read_int("Choice: ");
        if (ch == 0) return;
        int n = avl_count(students);
        switch (ch) {
        case 1: {
            if (!n) { printf("  No students.\n"); break; }
            Student *arr = malloc(sizeof(Student) * n);
            avl_to_array(students, arr, 0);
            merge_sort_students(arr, n);
            for (int i = 0; i < n; i++) {
                printf("  #%-3d", i + 1);
                print_student(&arr[i]);
            }
            free(arr);
            break;
        }
        case 2: {
            int k = read_int("How many numbers (max 50)? ");
            if (k < 1 || k > 50) { printf("  Out of range.\n"); break; }
            int a[50], b[50], c[50];
            for (int i = 0; i < k; i++) {
                char p[32]; sprintf(p, "  a[%d] = ", i);
                a[i] = b[i] = c[i] = read_int(p);
            }
            merge_sort(a, k); printf("  Merge sort (%ld comparisons):", sort_comparisons); print_arr(a, k);
            quick_sort(b, k); printf("  Quick sort (%ld comparisons):", sort_comparisons); print_arr(b, k);
            heap_sort(c, k);  printf("  Heap sort  (%ld comparisons):", sort_comparisons); print_arr(c, k);
            break;
        }
        case 3: {
            if (!n) { printf("  No students.\n"); break; }
            Student *arr = malloc(sizeof(Student) * n);
            int *rolls = malloc(sizeof(int) * n);
            avl_to_array(students, arr, 0);        /* inorder = sorted by roll */
            for (int i = 0; i < n; i++) rolls[i] = arr[i].roll;
            int key = read_int("Roll to find: ");
            int i1 = binary_search_iter(rolls, n, key);
            int i2 = binary_search_rec(rolls, 0, n - 1, key);
            printf("  Iterative index: %d   Recursive index: %d\n", i1, i2);
            if (i1 >= 0) print_student(&arr[i1]);
            free(arr); free(rolls);
            break;
        }
        case 4: {
            if (!n) { printf("  No students.\n"); break; }
            Student *arr = malloc(sizeof(Student) * n);
            int *g = malloc(sizeof(int) * n);
            avl_to_array(students, arr, 0);
            for (int i = 0; i < n; i++) g[i] = (int)(arr[i].cgpa * 100 + 0.5);
            int mx, mn, cmp = max_min_dc(g, 0, n - 1, &mx, &mn);
            printf("  Highest CGPA: %.2f   Lowest CGPA: %.2f\n", mx / 100.0, mn / 100.0);
            printf("  Comparisons: %d (naive scan would use %d)\n", cmp, 2 * (n - 1));
            free(arr); free(g);
            break;
        }
        case 5: {
            int d = read_int("Number of disks (1-10): ");
            if (d < 1 || d > 10) { printf("  Out of range.\n"); break; }
            long m = hanoi(d, 'A', 'C', 'B', 1);
            printf("  Total moves: %ld (= 2^%d - 1)\n", m, d);
            break;
        }
        case 6: run_benchmark(stdout); break;
        default: printf("  Invalid choice.\n");
        }
        pause_screen();
    }
}
