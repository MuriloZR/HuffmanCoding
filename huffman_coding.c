/*
=== TODO === :
[X] Implementar MinHeap, onde a frequência de cada caractere é o valor de prioridade
[] Implementar a Huffman Tree

=== ALGORITMO === :
1. Colocar todos os caracteres na Heap
2. Extrair os dois nós de menor frequência
3. Criar um nó que tem freqência igual à soma das freqências dos dois nós recém extraídos
4. Repetir os passos 2 e 3 até que a Heap possua apenas a raiz, a qual será a soma das freqências de todos os elementos
*/
#include <stdio.h>
#include <stdlib.h>

#define min(a, b) (a <= b ? a : b)

typedef struct par {
    int dado;
    int cont;
} par;

typedef struct no* No;
struct no {
    par dados;
    int index;
    No esq, dir;
};

#define DEFAULT_NO ((struct no){(par){-1,-1},-1,NULL,NULL})

// ===========================     HEAP     ============================== //
// ======================================================================= //
// A structure to represent a Min Heap
typedef struct MinHeap* Heap;
struct MinHeap {
    No harr;
    int capacity;
    int heap_size;
};

// Function prototypes
void swap(No x, No y);
Heap createMinHeap(int capacity);
void MinHeapify(Heap h, int i);
void deleteKey(Heap h, int i);
void insertKey(Heap h, struct no k);
struct no extractMin(Heap h);
void decreaseKey(Heap h, int i, int new_val);

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
    return h->harr[0].dados.cont;
}


Heap createMinHeap(int capacity) {
    Heap h = (Heap )malloc(sizeof(struct MinHeap));
    h->heap_size = 0;
    h->capacity = capacity;
    h->harr = (No)malloc(capacity * sizeof(struct no));
    return h;
}

// Retorna 1 se o no1 tiver maior prioridade
// Retorna -1 se o no2 tiver maior prioridade
int heap_prioridade(Heap h, int no1, int no2) {
    if(h->harr[no1].dados.cont != h->harr[no2].dados.cont) {
        if (h->harr[no1].dados.cont < h->harr[no2].dados.cont) {
            return 1;
        }
        else {
         return -1;
        }
    }
    else {
        if (h->harr[no1].index < h->harr[no2].index) {
            return 1;
        }
        else {
         return -1;
        }
    }
}

void insertKey(Heap h, struct no k) {
    if (h->heap_size == h->capacity) {
        printf("\nOverflow: Could not insertKey\n");
        return;
    }

    h->heap_size++;
    int i = h->heap_size - 1;
    h->harr[i] = k;

    // Continua enquanto o filho tiver prioridade maior
    while (i != 0 && heap_prioridade(h, parent(i), i) < 0) {
        swap(&h->harr[i], &h->harr[parent(i)]);
        i = parent(i);
    }
}

void decreaseKey(Heap h, int i, int new_val) {
    h->harr[i].dados.cont = new_val;
    // Continua enquanto o filho tiver prioridade maior
    while (i != 0 && heap_prioridade(h, parent(i), i) < 0) {
        swap(&h->harr[i], &h->harr[parent(i)]);
        i = parent(i);
    }
}

struct no extractMin(Heap h) {
    if (h->heap_size <= 0)
        return DEFAULT_NO;

    if (h->heap_size == 1) {
        h->heap_size--;
        return h->harr[0];
    }

    struct no root = h->harr[0];
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
        swap(&h->harr[i], &h->harr[smallest]);
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

struct no make_no(par p) {
    return (struct no){p,-1,NULL,NULL};
}

int main()
{
    Heap h = createMinHeap(11);
    insertKey(h, make_no((par){'a', 3}));
    insertKey(h, make_no((par){'b', 2}));
    insertKey(h, make_no((par){'c', 15}));
    insertKey(h, make_no((par){'d', 5}));
    insertKey(h, make_no((par){'e', 4}));
    insertKey(h, make_no((par){'f', 45}));
    printf("\n");

    while (h->heap_size > 0) {
        printf("%d ", extractMin(h).dados.cont);
    }

    free(h->harr);
    free(h);
    return 0;
}