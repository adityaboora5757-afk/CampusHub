#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 50

/* ---------- Input helpers (safe line-based reading) ---------- */
int  read_int(const char *prompt);
float read_float(const char *prompt);
void read_line(const char *prompt, char *buf, int size);
void print_line(void);
void pause_screen(void);

/* ---------- Generic integer queue (circular array) ----------
 * Used by BFS (graph) and level-order traversal (tree).        */
typedef struct {
    int *data;
    int front, rear, size, capacity;
} IntQueue;

IntQueue *iq_create(int capacity);
int  iq_empty(const IntQueue *q);
void iq_enqueue(IntQueue *q, int x);
int  iq_dequeue(IntQueue *q);
void iq_free(IntQueue *q);

/* ---------- Generic pointer stack (dynamic array) ----------
 * Used by iterative (non-recursive) tree traversal and search. */
typedef struct {
    void **data;
    int top, capacity;
} PtrStack;

PtrStack *ps_create(int capacity);
int   ps_empty(const PtrStack *s);
void  ps_push(PtrStack *s, void *p);
void *ps_pop(PtrStack *s);
void *ps_peek(const PtrStack *s);
void  ps_free(PtrStack *s);

#endif
