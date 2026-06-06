#include "heap.h"

void preOrder(No root, char** ans, char* curr, int depth, int* ans_idx);
char** huffmanCodes(char* s, int* freq, int n, int* returnSize);
char* codificarString(char* texto_original, char* caracteres_unicos, char** codigos_huffman, int tamanho_mapeamento);
char* decodificarString(char* texto_codificado, char* caracteres_unicos, char** codigos_huffman, int tamanho_mapeamento);