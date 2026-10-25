#include "common.h"

/* ================= Input helpers ================= */

void read_line(const char *prompt, char *buf, int size) {
    if (prompt) printf("%s", prompt);
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\n")] = '\0';
}

int read_int(const char *prompt) {
    char buf[64];
    char *end;
    for (;;) {
        read_line(prompt, buf, sizeof buf);
        if (feof(stdin)) exit(0);
        long v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0') return (int)v;
        printf("  Please enter a valid integer.\n");
    }
}

float read_float(const char *prompt) {
    char buf[64];
    char *end;
    for (;;) {
        read_line(prompt, buf, sizeof buf);
        if (feof(stdin)) exit(0);
        float v = strtof(buf, &end);
        if (end != buf && *end == '\0') return v;
        printf("  Please enter a valid number.\n");
    }
}

void print_line(void) {
    printf("------------------------------------------------------------\n");
}

void pause_screen(void) {
    char buf[8];
    read_line("\nPress Enter to continue...", buf, sizeof buf);
}

/* ================= Integer queue (circular) ================= */

IntQueue *iq_create(int capacity) {
    IntQueue *q = malloc(sizeof *q);
    q->data = malloc(sizeof(int) * capacity);
    q->front = 0; q->rear = -1; q->size = 0; q->capacity = capacity;
    return q;
}

int iq_empty(const IntQueue *q) { return q->size == 0; }

void iq_enqueue(IntQueue *q, int x) {
    if (q->size == q->capacity) {           /* grow: unroll into new buffer */
        int *nd = malloc(sizeof(int) * q->capacity * 2);
        for (int i = 0; i < q->size; i++)
            nd[i] = q->data[(q->front + i) % q->capacity];
        free(q->data);
        q->data = nd; q->front = 0; q->rear = q->size - 1;
        q->capacity *= 2;
    }
    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = x;
    q->size++;
}

int iq_dequeue(IntQueue *q) {
    int x = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return x;
}

void iq_free(IntQueue *q) { free(q->data); free(q); }

/* ================= Pointer stack ================= */

PtrStack *ps_create(int capacity) {
    PtrStack *s = malloc(sizeof *s);
    s->data = malloc(sizeof(void *) * capacity);
    s->top = -1; s->capacity = capacity;
    return s;
}

int ps_empty(const PtrStack *s) { return s->top == -1; }

void ps_push(PtrStack *s, void *p) {
    if (s->top + 1 == s->capacity) {
        s->capacity *= 2;
        s->data = realloc(s->data, sizeof(void *) * s->capacity);
    }
    s->data[++s->top] = p;
}

void *ps_pop(PtrStack *s)  { return s->data[s->top--]; }
void *ps_peek(const PtrStack *s) { return s->data[s->top]; }
void  ps_free(PtrStack *s) { free(s->data); free(s); }
