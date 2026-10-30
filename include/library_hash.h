#ifndef LIBRARY_HASH_H
#define LIBRARY_HASH_H

#include "common.h"

/* Module 2: Library catalogue in a hash table keyed by Book ID.
 * Covers syllabus classes 29-31 (indexing, hashing, collision
 * resolution: linear probing, quadratic probing, double hashing,
 * and separate chaining). */

#define TABLE_SIZE 31   /* prime size spreads keys evenly */

typedef enum { LINEAR = 1, QUADRATIC, DOUBLE_HASH, CHAINING } ProbeMode;
typedef enum { EMPTY = 0, OCCUPIED, DELETED } SlotState;

typedef struct {
    int  id;
    char title[NAME_LEN];
    char author[NAME_LEN];
    int  available;
} Book;

typedef struct ChainNode {
    Book b;
    struct ChainNode *next;
} ChainNode;

typedef struct {
    ProbeMode  mode;
    Book       slots[TABLE_SIZE];       /* open addressing        */
    SlotState  state[TABLE_SIZE];
    ChainNode *chains[TABLE_SIZE];      /* separate chaining      */
    int        count;
    long       total_probes;            /* for average-probe stats */
    int        collisions;
} HashTable;

void ht_init(HashTable *ht, ProbeMode mode);
int  ht_insert(HashTable *ht, Book b, int *probes);   /* 1 ok, 0 dup/full */
Book *ht_search(HashTable *ht, int id, int *probes);
int  ht_delete(HashTable *ht, int id);
void ht_display(const HashTable *ht);
void ht_free(HashTable *ht);
double ht_load_factor(const HashTable *ht);
const char *ht_mode_name(ProbeMode m);

void library_menu(HashTable *ht);

#endif
