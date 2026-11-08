#ifndef SEED_DATA_H
#define SEED_DATA_H

#include "student_avl.h"
#include "library_hash.h"
#include "campus_graph.h"
#include "helpdesk_heap.h"

/* Loads sample data so every module can be demonstrated immediately. */
AVLNode *seed_students(AVLNode *root);
void seed_library(HashTable *ht);
void seed_campus(Graph *g);
void seed_helpdesk(MaxHeap *h);

#endif
