#ifndef STUDENT_AVL_H
#define STUDENT_AVL_H

#include "common.h"

/* Module 1: Student Records stored in a self-balancing AVL tree
 * keyed by roll number. Covers syllabus classes 7-15
 * (trees, BST insert/delete/search/traversal, AVL rotations). */

typedef struct {
    int   roll;
    char  name[NAME_LEN];
    float cgpa;
} Student;

typedef struct AVLNode {
    Student s;
    int height;
    struct AVLNode *left, *right;
} AVLNode;

/* Core operations */
AVLNode *avl_insert(AVLNode *root, Student s, int *ok);
AVLNode *avl_delete(AVLNode *root, int roll, int *ok, Student *removed);
AVLNode *avl_search_rec(AVLNode *root, int roll);
AVLNode *avl_search_iter(AVLNode *root, int roll);

/* Traversals */
void avl_inorder_rec(const AVLNode *root);
void avl_preorder_rec(const AVLNode *root);
void avl_postorder_rec(const AVLNode *root);
void avl_inorder_iter(AVLNode *root);   /* uses an explicit stack  */
void avl_level_order(AVLNode *root);    /* uses a queue            */

/* Utilities */
int  avl_height(const AVLNode *n);
int  avl_count(const AVLNode *root);
int  avl_is_balanced(const AVLNode *root);   /* verifies AVL property */
int  avl_to_array(const AVLNode *root, Student *arr, int idx);
void avl_print_tree(const AVLNode *root, int depth);
void avl_free(AVLNode *root);
void print_student(const Student *s);

/* Rotation counter (for the report / demo) */
extern int avl_rotation_count;

void student_menu(AVLNode **root);

#endif
