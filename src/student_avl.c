#include "student_avl.h"

int avl_rotation_count = 0;

/* ================= Helpers ================= */

int avl_height(const AVLNode *n) { return n ? n->height : 0; }

static int max_i(int a, int b) { return a > b ? a : b; }

static void update_height(AVLNode *n) {
    n->height = 1 + max_i(avl_height(n->left), avl_height(n->right));
}

static int balance_factor(const AVLNode *n) {
    return n ? avl_height(n->left) - avl_height(n->right) : 0;
}

static AVLNode *new_node(Student s) {
    AVLNode *n = malloc(sizeof *n);
    n->s = s; n->height = 1; n->left = n->right = NULL;
    return n;
}

void print_student(const Student *s) {
    printf("  Roll: %-6d  Name: %-20s  CGPA: %.2f\n", s->roll, s->name, s->cgpa);
}

/* ================= Rotations =================
 *        y                x
 *       / \   right     /   \
 *      x   T3  ----->  T1    y
 *     / \     <-----        / \
 *    T1  T2    left        T2  T3
 */
static AVLNode *rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;
    x->right = y;
    y->left = T2;
    update_height(y);
    update_height(x);
    avl_rotation_count++;
    return x;
}

static AVLNode *rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;
    y->left = x;
    x->right = T2;
    update_height(x);
    update_height(y);
    avl_rotation_count++;
    return y;
}

/* Restore balance at node n after an insert/delete below it. */
static AVLNode *rebalance(AVLNode *n) {
    update_height(n);
    int bf = balance_factor(n);

    if (bf > 1) {                                   /* left heavy  */
        if (balance_factor(n->left) < 0)            /* LR case     */
            n->left = rotate_left(n->left);
        return rotate_right(n);                     /* LL case     */
    }
    if (bf < -1) {                                  /* right heavy */
        if (balance_factor(n->right) > 0)           /* RL case     */
            n->right = rotate_right(n->right);
        return rotate_left(n);                      /* RR case     */
    }
    return n;
}

/* ================= Insert (recursive) ================= */

AVLNode *avl_insert(AVLNode *root, Student s, int *ok) {
    if (!root) { *ok = 1; return new_node(s); }
    if (s.roll < root->s.roll)
        root->left = avl_insert(root->left, s, ok);
    else if (s.roll > root->s.roll)
        root->right = avl_insert(root->right, s, ok);
    else { *ok = 0; return root; }                  /* duplicate key */
    return rebalance(root);
}

/* ================= Delete (recursive) ================= */

static AVLNode *min_node(AVLNode *n) {
    while (n->left) n = n->left;
    return n;
}

AVLNode *avl_delete(AVLNode *root, int roll, int *ok, Student *removed) {
    if (!root) { *ok = 0; return NULL; }

    if (roll < root->s.roll)
        root->left = avl_delete(root->left, roll, ok, removed);
    else if (roll > root->s.roll)
        root->right = avl_delete(root->right, roll, ok, removed);
    else {
        *ok = 1;
        if (removed) *removed = root->s;
        /* Case 1 & 2: zero or one child */
        if (!root->left || !root->right) {
            AVLNode *child = root->left ? root->left : root->right;
            free(root);
            return child;
        }
        /* Case 3: two children -> copy inorder successor, delete it */
        AVLNode *succ = min_node(root->right);
        root->s = succ->s;
        root->right = avl_delete(root->right, succ->s.roll, ok, NULL);
        *ok = 1;
    }
    return rebalance(root);
}

/* ================= Search ================= */

AVLNode *avl_search_rec(AVLNode *root, int roll) {
    if (!root || root->s.roll == roll) return root;
    return roll < root->s.roll ? avl_search_rec(root->left, roll)
                               : avl_search_rec(root->right, roll);
}

AVLNode *avl_search_iter(AVLNode *root, int roll) {
    while (root && root->s.roll != roll)
        root = roll < root->s.roll ? root->left : root->right;
    return root;
}

/* ================= Traversals ================= */

void avl_inorder_rec(const AVLNode *r) {
    if (!r) return;
    avl_inorder_rec(r->left);
    print_student(&r->s);
    avl_inorder_rec(r->right);
}

void avl_preorder_rec(const AVLNode *r) {
    if (!r) return;
    print_student(&r->s);
    avl_preorder_rec(r->left);
    avl_preorder_rec(r->right);
}

void avl_postorder_rec(const AVLNode *r) {
    if (!r) return;
    avl_postorder_rec(r->left);
    avl_postorder_rec(r->right);
    print_student(&r->s);
}

/* Non-recursive inorder using an explicit stack. */
void avl_inorder_iter(AVLNode *root) {
    PtrStack *st = ps_create(16);
    AVLNode *cur = root;
    while (cur || !ps_empty(st)) {
        while (cur) { ps_push(st, cur); cur = cur->left; }
        cur = ps_pop(st);
        print_student(&cur->s);
        cur = cur->right;
    }
    ps_free(st);
}

/* Level-order (BFS on tree). Queue stores indices into a node table. */
void avl_level_order(AVLNode *root) {
    if (!root) return;
    int n = avl_count(root);
    AVLNode **table = malloc(sizeof(AVLNode *) * n);
    int used = 0, level = 0;
    IntQueue *q = iq_create(n);
    table[used] = root; iq_enqueue(q, used++);
    while (!iq_empty(q)) {
        int cnt = q->size;
        printf("  Level %d:", level++);
        while (cnt--) {
            AVLNode *x = table[iq_dequeue(q)];
            printf(" %d", x->s.roll);
            if (x->left)  { table[used] = x->left;  iq_enqueue(q, used++); }
            if (x->right) { table[used] = x->right; iq_enqueue(q, used++); }
        }
        printf("\n");
    }
    iq_free(q);
    free(table);
}

/* ================= Utilities ================= */

int avl_count(const AVLNode *r) {
    return r ? 1 + avl_count(r->left) + avl_count(r->right) : 0;
}

int avl_is_balanced(const AVLNode *r) {
    if (!r) return 1;
    int bf = avl_height(r->left) - avl_height(r->right);
    if (bf > 1 || bf < -1) return 0;
    if (r->left  && r->left->s.roll  >= r->s.roll) return 0;
    if (r->right && r->right->s.roll <= r->s.roll) return 0;
    return avl_is_balanced(r->left) && avl_is_balanced(r->right);
}

int avl_to_array(const AVLNode *r, Student *arr, int idx) {
    if (!r) return idx;
    idx = avl_to_array(r->left, arr, idx);
    arr[idx++] = r->s;
    return avl_to_array(r->right, arr, idx);
}

/* Sideways tree print: right subtree on top. */
void avl_print_tree(const AVLNode *r, int depth) {
    if (!r) return;
    avl_print_tree(r->right, depth + 1);
    for (int i = 0; i < depth; i++) printf("       ");
    printf("%d(h%d)\n", r->s.roll, r->height);
    avl_print_tree(r->left, depth + 1);
}

void avl_free(AVLNode *r) {
    if (!r) return;
    avl_free(r->left);
    avl_free(r->right);
    free(r);
}

/* ================= Menu ================= */

void student_menu(AVLNode **root) {
    /* Stack of deleted students lets the user undo deletions (LIFO). */
    static Student undo[100];
    static int undo_top = -1;

    for (;;) {
        print_line();
        printf(" STUDENT RECORDS  (AVL Tree)   students: %d   height: %d\n",
               avl_count(*root), avl_height(*root));
        print_line();
        printf(" 1. Add student            2. Delete student\n");
        printf(" 3. Search (recursive)     4. Search (iterative)\n");
        printf(" 5. Inorder (recursive)    6. Inorder (non-recursive, stack)\n");
        printf(" 7. Preorder               8. Postorder\n");
        printf(" 9. Level order (queue)   10. Show tree structure\n");
        printf("11. Undo last delete      0. Back\n");
        int ch = read_int("Choice: ");
        int ok;
        if (ch == 0) return;
        switch (ch) {
        case 1: {
            Student s;
            s.roll = read_int("Roll number: ");
            read_line("Name: ", s.name, NAME_LEN);
            s.cgpa = read_float("CGPA: ");
            int before = avl_rotation_count;
            *root = avl_insert(*root, s, &ok);
            if (ok) printf("  Inserted. Rotations performed: %d\n",
                           avl_rotation_count - before);
            else    printf("  Roll %d already exists.\n", s.roll);
            break;
        }
        case 2: {
            Student removed;
            int roll = read_int("Roll to delete: ");
            *root = avl_delete(*root, roll, &ok, &removed);
            if (ok) {
                printf("  Deleted %s.\n", removed.name);
                if (undo_top < 99) undo[++undo_top] = removed;
            } else printf("  Not found.\n");
            break;
        }
        case 3: case 4: {
            int roll = read_int("Roll to search: ");
            AVLNode *n = (ch == 3) ? avl_search_rec(*root, roll)
                                   : avl_search_iter(*root, roll);
            if (n) print_student(&n->s); else printf("  Not found.\n");
            break;
        }
        case 5: avl_inorder_rec(*root);  break;
        case 6: avl_inorder_iter(*root); break;
        case 7: avl_preorder_rec(*root); break;
        case 8: avl_postorder_rec(*root); break;
        case 9: avl_level_order(*root);  break;
        case 10: avl_print_tree(*root, 0); break;
        case 11:
            if (undo_top < 0) { printf("  Nothing to undo.\n"); break; }
            *root = avl_insert(*root, undo[undo_top], &ok);
            printf("  Restored %s.\n", undo[undo_top--].name);
            break;
        default: printf("  Invalid choice.\n");
        }
        pause_screen();
    }
}
