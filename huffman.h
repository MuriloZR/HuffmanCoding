#include "heap.h"

// Percorre a árvore em pré-ordem para achar as codificações dos caracteres
void preOrder(No root, char** ans, char* curr, int depth, int* ans_idx);

// Gera um registro dos códigos de Huffman de cada caractere na árvore
char** huffmanCodes(char* s, int* freq, int n, int* returnSize);

// Codifica uma string para sua forma na representação em códigos de Huffman
char* codificarString(char* texto_original, char* caracteres_unicos, char** codigos_huffman, int tamanho_mapeamento);

// Decodifica uma string para sua forma na representação normal
char* decodificarString(char* texto_codificado, char* caracteres_unicos, char** codigos_huffman, int tamanho_mapeamento);