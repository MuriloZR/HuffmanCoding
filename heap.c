#include "heap.h"

int parent(int i) {
    return (i - 1) / 2;
}

int left(int i) {
    return (2 * i + 1);
}

int right(int i) {
    return (2 * i + 2);
}

int getMin(Heap h) {
    return h->harr[0]->dados.cont;
}

Heap createMinHeap(int capacity) {
    Heap h = (Heap )malloc(sizeof(struct MinHeap));
    h->heap_size = 0;
    h->capacity = capacity;
    h->harr = (No *)malloc(capacity * sizeof(No));
    return h;
}

// Retorna 1 se o no1 tiver maior prioridade
// Retorna -1 se o no2 tiver maior prioridade
int heap_prioridade(Heap h, int no1, int no2) {
    if(h->harr[no1]->dados.cont != h->harr[no2]->dados.cont) {
        if (h->harr[no1]->dados.cont < h->harr[no2]->dados.cont) {
            return 1;
        }
        else {
         return -1;
        }
    }
    else {
        if (h->harr[no1]->index < h->harr[no2]->index) {
            return 1;
        }
        else {
         return -1;
        }
    }
}

void insertKey(Heap h, No k) {
    if (h->heap_size == h->capacity) {
        fprintf(stderr,"\nERROR: Overflow: Could not insertKey\n");
        return;
    }

    h->heap_size++;
    int i = h->heap_size - 1;
    h->harr[i] = k;

    // Continua enquanto o filho tiver prioridade maior
    while (i != 0 && heap_prioridade(h, parent(i), i) < 0) {
        swap(h->harr[i], h->harr[parent(i)]);
        i = parent(i);
    }
}

void decreaseKey(Heap h, int i, int new_val) {
    h->harr[i]->dados.cont = new_val;

    // Continua enquanto o filho tiver prioridade maior
    while (i != 0 && heap_prioridade(h, parent(i), i) < 0) {
        swap(h->harr[i], h->harr[parent(i)]);
        i = parent(i);
    }
}

No extractMin(Heap h) {
    if (h->heap_size <= 0)
        return NULL;

    if (h->heap_size == 1) {
        h->heap_size--;
        return h->harr[0];
    }

    No root = h->harr[0];
    h->harr[0] = h->harr[h->heap_size - 1];
    h->heap_size--;
    MinHeapify(h, 0);

    return root;
}

void deleteKey(Heap h, int i) {
    decreaseKey(h, i, -1);
    extractMin(h);
}

void MinHeapify(Heap h, int i) {
    int l = left(i);
    int r = right(i);
    int smallest = i;

    if (l < h->heap_size && heap_prioridade(h, l, i) > 0)
        smallest = l;

    if (r < h->heap_size && heap_prioridade(h, r, smallest) > 0)
        smallest = r;

    if (smallest != i) {
        swap(h->harr[i], h->harr[smallest]);
        MinHeapify(h, smallest);
    }
}

void swap(No x, No y) {
    struct no temp = *x;
    *x = *y;
    *y = temp;
}

No create_no(par dado, int i) {
    No no = (No)malloc(sizeof(*no));
    no->dados = dado;
    no->index = i;
    no->esq = no->dir = NULL;
    return no;
}

No create_internal(No e, No d) {
    No no = create_no((par){-1, e->dados.cont + d->dados.cont}, min(e->index, d->index));
    no->esq = e;
    no->dir = d;
    return no;
}

void free_rec(No no) {
    if (no == NULL) return;

    free_rec(no->esq);
    free_rec(no->dir);
    free(no);
}