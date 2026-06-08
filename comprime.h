#ifndef COMPRIME_H
#define COMPRIME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap.h"

#define DICT_SIZE 256

// Estrutura para gerenciar os códigos de Huffman
typedef struct {
    unsigned int bits; // Os bits que representam o código
    int tamanho;       // Quantos bits esse código tem
} CodigoHuffman;

// Estrutura para gerenciar os bits em um arquivo
typedef struct gerenciador {
    FILE *arquivo; // O arquivo a ser processado
    uint8_t byte; // O byte a ser lido/escrito
    int contagem_bits; // Quantos bits já foram preenchidos no byte
} GerenciadorDeBits;

GerenciadorDeBits* criarGerenciador(FILE *arq);

// === Compressão === //

// Função que transforma os bytes do arquivo em suas versões codificadas
void escreverCodigo(GerenciadorDeBits *escritor, unsigned int codigo, int tamanho);

// Grava qualquer bit que tenha sobrado no buffer e libera o escritor
void finalizarEscritor(GerenciadorDeBits *escritor);

// Preenche um array de 256 posições com a frequência de cada byte no arquivo
int calcularFrequencias(const char *nome_arquivo, int *frequencias);

// Constrói o dicionário com os códigos de Huffman
CodigoHuffman* construirDicionario(int *frequencias);

// Lê o arquivo original e gera a versão comprimida
int comprimirArquivo(const char *arquivo_entrada, const char *arquivo_saida, CodigoHuffman *dicionario, int *frequencias);

// === Descompressão === //

// Constrói uma árvore de Huffman a partir de um array de frequências
No construirArvore(int *frequencias);

// Lê um arquivo comprimido e reconstrói ele para sua forma original
int descomprimirArquivo(const char *arquivo_comprimido, const char *arquivo_saida);

#endif