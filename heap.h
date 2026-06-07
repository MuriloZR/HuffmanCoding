#pragma once
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

// Troca dois nós
void swap(No x, No y);

// cria uma heap vazia com capacidade fixa
Heap createMinHeap(int capacity);

void MinHeapify(Heap h, int i);

// Deleta o nó de índice i
void deleteKey(Heap h, int i);

// Insere um nó k na heap
void insertKey(Heap h, No k);

// Extrai a raiz da heap
No extractMin(Heap h);

// Altera o valor do nó de índice i para new_val
void decreaseKey(Heap h, int i, int new_val);

// Cria um nó vazio
No create_no(par dado, int i);

// Cria um nó interno para a Huffman Tree
No create_internal(No e, No d);

// Libera toda a memória a partir de um nó inicial
void free_rec(No no);