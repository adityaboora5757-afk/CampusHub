#include "helpdesk_heap.h"

/* a "beats" b if it has higher priority, or same priority but came first */
static int beats(const Ticket *a, const Ticket *b) {
    if (a->priority != b->priority) return a->priority > b->priority;
    return a->arrival < b->arrival;
}

static void swap_t(Ticket *x, Ticket *y) { Ticket t = *x; *x = *y; *y = t; }

static void sift_up(MaxHeap *h, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (!beats(&h->a[i], &h->a[p])) break;
        swap_t(&h->a[i], &h->a[p]);
        i = p;
    }
}

static void sift_down(MaxHeap *h, int i) {
    for (;;) {
        int l = 2 * i + 1, r = l + 1, best = i;
        if (l < h->size && beats(&h->a[l], &h->a[best])) best = l;
        if (r < h->size && beats(&h->a[r], &h->a[best])) best = r;
        if (best == i) return;
        swap_t(&h->a[i], &h->a[best]);
        i = best;
    }
}

void heap_init(MaxHeap *h) { h->size = 0; h->clock = 0; h->next_id = 1001; }

int heap_push(MaxHeap *h, int priority, const char *text) {
    if (h->size >= MAX_TICKETS || priority < 1 || priority > 5) return 0;
    Ticket *t = &h->a[h->size];
    t->id = h->next_id++;
    t->priority = priority;
    t->arrival = h->clock++;
    strncpy(t->text, text, NAME_LEN - 1);
    t->text[NAME_LEN - 1] = '\0';
    sift_up(h, h->size++);
    return t->id;
}

int heap_peek(const MaxHeap *h, Ticket *out) {
    if (h->size == 0) return 0;
    *out = h->a[0];
    return 1;
}

int heap_pop(MaxHeap *h, Ticket *out) {
    if (h->size == 0) return 0;
    *out = h->a[0];
    h->a[0] = h->a[--h->size];
    sift_down(h, 0);
    return 1;
}

void heap_build(MaxHeap *h) {
    for (int i = h->size / 2 - 1; i >= 0; i--) sift_down(h, i);
}

int heap_is_valid(const MaxHeap *h) {
    for (int i = 1; i < h->size; i++)
        if (beats(&h->a[i], &h->a[(i - 1) / 2])) return 0;
    return 1;
}

void heap_display(const MaxHeap *h) {
    if (!h->size) { printf("  No pending tickets.\n"); return; }
    printf("  Heap array (index: id/priority):\n ");
    for (int i = 0; i < h->size; i++)
        printf(" [%d] %d/P%d", i, h->a[i].id, h->a[i].priority);
    printf("\n");
}

static const char *prio_name(int p) {
    static const char *n[] = {"", "Low", "Minor", "Normal", "High", "Critical"};
    return (p >= 1 && p <= 5) ? n[p] : "?";
}

void helpdesk_menu(MaxHeap *h) {
    for (;;) {
        print_line();
        printf(" HELP DESK  (Max-Heap priority queue)   pending: %d\n", h->size);
        print_line();
        printf(" 1. Raise ticket       2. Serve next ticket\n");
        printf(" 3. Peek next ticket   4. Show heap array\n");
        printf(" 0. Back\n");
        int ch = read_int("Choice: ");
        Ticket t;
        if (ch == 0) return;
        switch (ch) {
        case 1: {
            char text[NAME_LEN];
            read_line("Complaint: ", text, NAME_LEN);
            int p = read_int("Priority 1-5 (5 = critical): ");
            int id = heap_push(h, p, text);
            if (id) printf("  Ticket #%d raised.\n", id);
            else printf("  Invalid priority or queue full.\n");
            break;
        }
        case 2: case 3:
            if (!(ch == 2 ? heap_pop(h, &t) : heap_peek(h, &t))) {
                printf("  No pending tickets.\n"); break;
            }
            printf("  %s #%d [%s] %s\n", ch == 2 ? "Serving" : "Next:",
                   t.id, prio_name(t.priority), t.text);
            break;
        case 4: heap_display(h); break;
        default: printf("  Invalid choice.\n");
        }
        pause_screen();
    }
}
