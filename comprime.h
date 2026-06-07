#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // Para usar uint8_t (byte exato de 8 bits)
#include "heap.h"

#define DICT_SIZE 256

// Estrutura para gerenciar os bits em um arquivo
typedef struct gerenciador {
    FILE *arquivo;
    uint8_t byte; // O byte que estamos "enchendo" com bits
    int contagem_bits;  // Quantos bits já colocamos no byte_atual (0 a 8)
} GerenciadorDeBits;

// Inicia o escritor associado a um arquivo já aberto para escrita binária ("wb")
GerenciadorDeBits* criarGerenciador(FILE *arq);

// === Compressão === //

// Função que converte uma string "0101" em bits reais e escreve no arquivo
void escreverCodigo(GerenciadorDeBits *escritor, const char *codigo_huffman);

// Grava qualquer bit que tenha sobrado no buffer e libera o escritor
void finalizarEscritor(GerenciadorDeBits *escritor);

// Preenche um array de 256 posições com a frequência de cada byte no arquivo
int calcularFrequencias(const char *nome_arquivo, int *frequencias);

// Constrói o dicionário com os códigos de Huffman
char** construirDicionario(int *frequencias);

// Lê o arquivo original e gera a versão comprimida (.huff)
int comprimirArquivo(const char *arquivo_entrada, const char *arquivo_saida, char **dicionario, int *frequencias);

// === Descompressão === //

// Constrói uma árvore de Huffman a partir de um array de frequências
No construirArvore(int *frequencias);

// Lê um arquivo .huff e reconstrói ele para sua forma original
int descomprimirArquivo(const char *arquivo_comprimido, const char *arquivo_saida);