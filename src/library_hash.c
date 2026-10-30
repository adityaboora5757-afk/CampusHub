#include "library_hash.h"

/* ================= Hash functions ================= */

static int h1(int key) { return key % TABLE_SIZE; }          /* division method */
static int h2(int key) { return 7 - (key % 7); }             /* never 0, < size */

/* i-th probe position for a key, depending on the mode */
static int probe(ProbeMode m, int key, int i) {
    switch (m) {
    case LINEAR:      return (h1(key) + i) % TABLE_SIZE;
    case QUADRATIC:   return (h1(key) + i * i) % TABLE_SIZE;
    case DOUBLE_HASH: return (h1(key) + i * h2(key)) % TABLE_SIZE;
    default:          return h1(key);
    }
}

const char *ht_mode_name(ProbeMode m) {
    switch (m) {
    case LINEAR: return "Linear probing";
    case QUADRATIC: return "Quadratic probing";
    case DOUBLE_HASH: return "Double hashing";
    case CHAINING: return "Separate chaining";
    }
    return "?";
}

void ht_init(HashTable *ht, ProbeMode mode) {
    memset(ht, 0, sizeof *ht);
    ht->mode = mode;
}

double ht_load_factor(const HashTable *ht) {
    return (double)ht->count / TABLE_SIZE;
}

/* ================= Insert ================= */

int ht_insert(HashTable *ht, Book b, int *probes) {
    *probes = 0;
    if (b.id < 0) return 0;

    if (ht->mode == CHAINING) {
        int idx = h1(b.id);
        for (ChainNode *c = ht->chains[idx]; c; c = c->next) {
            (*probes)++;
            if (c->b.id == b.id) return 0;
        }
        (*probes)++;
        if (ht->chains[idx]) ht->collisions++;
        ChainNode *n = malloc(sizeof *n);
        n->b = b; n->next = ht->chains[idx];   /* insert at head */
        ht->chains[idx] = n;
        ht->count++; ht->total_probes += *probes;
        return 1;
    }

    /* Open addressing: reuse first DELETED slot, but keep scanning to
     * make sure the key is not already present further along. */
    int first_free = -1;
    for (int i = 0; i < TABLE_SIZE; i++) {
        int pos = probe(ht->mode, b.id, i);
        (*probes)++;
        if (ht->state[pos] == EMPTY) {
            if (first_free < 0) first_free = pos;
            break;
        }
        if (ht->state[pos] == DELETED) {
            if (first_free < 0) first_free = pos;
        } else if (ht->slots[pos].id == b.id) {
            return 0;                               /* duplicate */
        }
    }
    if (first_free < 0) return 0;                   /* table full / cycle */
    if (*probes > 1) ht->collisions++;
    ht->slots[first_free] = b;
    ht->state[first_free] = OCCUPIED;
    ht->count++; ht->total_probes += *probes;
    return 1;
}

/* ================= Search ================= */

Book *ht_search(HashTable *ht, int id, int *probes) {
    *probes = 0;
    if (ht->mode == CHAINING) {
        for (ChainNode *c = ht->chains[h1(id)]; c; c = c->next) {
            (*probes)++;
            if (c->b.id == id) return &c->b;
        }
        return NULL;
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
        int pos = probe(ht->mode, id, i);
        (*probes)++;
        if (ht->state[pos] == EMPTY) return NULL;   /* end of cluster */
        if (ht->state[pos] == OCCUPIED && ht->slots[pos].id == id)
            return &ht->slots[pos];
    }
    return NULL;
}

/* ================= Delete ================= */

int ht_delete(HashTable *ht, int id) {
    if (ht->mode == CHAINING) {
        ChainNode **pp = &ht->chains[h1(id)];
        while (*pp) {
            if ((*pp)->b.id == id) {
                ChainNode *dead = *pp;
                *pp = dead->next;
                free(dead);
                ht->count--;
                return 1;
            }
            pp = &(*pp)->next;
        }
        return 0;
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
        int pos = probe(ht->mode, id, i);
        if (ht->state[pos] == EMPTY) return 0;
        if (ht->state[pos] == OCCUPIED && ht->slots[pos].id == id) {
            ht->state[pos] = DELETED;               /* tombstone */
            ht->count--;
            return 1;
        }
    }
    return 0;
}

/* ================= Display / free ================= */

void ht_display(const HashTable *ht) {
    printf("  Mode: %s | books: %d | load factor: %.2f | collisions: %d\n",
           ht_mode_name(ht->mode), ht->count, ht_load_factor(ht), ht->collisions);
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (ht->mode == CHAINING) {
            if (!ht->chains[i]) continue;
            printf("  [%2d]", i);
            for (ChainNode *c = ht->chains[i]; c; c = c->next)
                printf(" -> %d(%s)", c->b.id, c->b.title);
            printf("\n");
        } else if (ht->state[i] == OCCUPIED) {
            printf("  [%2d] %5d  %-28s %-18s %s\n", i, ht->slots[i].id,
                   ht->slots[i].title, ht->slots[i].author,
                   ht->slots[i].available ? "available" : "issued");
        } else if (ht->state[i] == DELETED) {
            printf("  [%2d]  <deleted>\n", i);
        }
    }
}

void ht_free(HashTable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        ChainNode *c = ht->chains[i];
        while (c) { ChainNode *n = c->next; free(c); c = n; }
        ht->chains[i] = NULL;
    }
}

/* ================= Menu ================= */

void library_menu(HashTable *ht) {
    for (;;) {
        print_line();
        printf(" LIBRARY  (Hash Table, %s)\n", ht_mode_name(ht->mode));
        print_line();
        printf(" 1. Add book        2. Search book     3. Remove book\n");
        printf(" 4. Issue book      5. Return book     6. Show table\n");
        printf(" 0. Back\n");
        int ch = read_int("Choice: "), probes;
        if (ch == 0) return;
        switch (ch) {
        case 1: {
            Book b;
            b.id = read_int("Book ID: ");
            read_line("Title: ", b.title, NAME_LEN);
            read_line("Author: ", b.author, NAME_LEN);
            b.available = 1;
            if (ht_insert(ht, b, &probes))
                printf("  Added after %d probe(s).\n", probes);
            else printf("  Could not add (duplicate ID or table full).\n");
            break;
        }
        case 2: case 4: case 5: {
            int id = read_int("Book ID: ");
            Book *b = ht_search(ht, id, &probes);
            if (!b) { printf("  Not found (%d probes).\n", probes); break; }
            if (ch == 2)
                printf("  Found in %d probe(s): %s by %s [%s]\n", probes,
                       b->title, b->author, b->available ? "available" : "issued");
            else if (ch == 4) {
                if (b->available) { b->available = 0; printf("  Issued.\n"); }
                else printf("  Already issued.\n");
            } else {
                if (!b->available) { b->available = 1; printf("  Returned.\n"); }
                else printf("  Book was not issued.\n");
            }
            break;
        }
        case 3:
            printf(ht_delete(ht, read_int("Book ID: ")) ? "  Removed.\n" : "  Not found.\n");
            break;
        case 6: ht_display(ht); break;
        default: printf("  Invalid choice.\n");
        }
        pause_screen();
    }
}
