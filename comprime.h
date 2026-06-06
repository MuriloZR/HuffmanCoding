#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // Para usarmos uint8_t (byte exato de 8 bits)

#define DICT_SIZE 256

// Estrutura para gerenciar a escrita de bits em um arquivo
typedef struct {
    FILE *arquivo;
    uint8_t byte; // O byte que estamos "enchendo" com bits
    int contagem_bits;  // Quantos bits já colocamos no byte_atual (0 a 8)
} EscritorDeBits;

EscritorDeBits* criarEscritor(FILE *arq);
void escreverCodigo(EscritorDeBits *escritor, const char *codigo_huffman);
void finalizarEscritor(EscritorDeBits *escritor);