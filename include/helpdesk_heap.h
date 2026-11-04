#ifndef HELPDESK_HEAP_H
#define HELPDESK_HEAP_H

#include "common.h"

/* Module 4: Help Desk. Complaints are served by urgency using a
 * binary MAX-HEAP (priority queue). Ties are broken by arrival order
 * (earlier ticket first). Covers syllabus classes 21-22 (heap,
 * build-heap, heap operations). */

#define MAX_TICKETS 200

typedef struct {
    int  id;
    int  priority;          /* 1 (low) .. 5 (critical) */
    long arrival;           /* tie-breaker              */
    char text[NAME_LEN];
} Ticket;

typedef struct {
    Ticket a[MAX_TICKETS];
    int    size;
    long   clock;
    int    next_id;
} MaxHeap;

void heap_init(MaxHeap *h);
int  heap_push(MaxHeap *h, int priority, const char *text);
int  heap_pop(MaxHeap *h, Ticket *out);
int  heap_peek(const MaxHeap *h, Ticket *out);
void heap_build(MaxHeap *h);            /* Floyd's O(n) build-heap */
int  heap_is_valid(const MaxHeap *h);
void heap_display(const MaxHeap *h);

void helpdesk_menu(MaxHeap *h);

#endif
