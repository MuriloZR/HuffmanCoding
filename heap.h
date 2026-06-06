#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// A structure to represent a Min Heap
typedef struct MinHeap* Heap;
struct MinHeap {
    No *harr;
    int capacity;
    int heap_size;
};

// Function prototypes
void swap(No x, No y);
Heap createMinHeap(int capacity);
void MinHeapify(Heap h, int i);
void deleteKey(Heap h, int i);
void insertKey(Heap h, No k);
No extractMin(Heap h);
void decreaseKey(Heap h, int i, int new_val);
No create_no(par dado, int i);
No create_internal(No e, No d);
void free_rec(No no);
