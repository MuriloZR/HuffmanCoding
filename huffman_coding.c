#include "huffman.h"
#include "comprime.h"

int main() {
    const char *nome_original = "entrada.txt";
    const char *nome_comprimido = "saida.huff";

    // ==========================================================
    // PASSO 0: Criar um arquivo de teste (Apenas para demonstração)
    // ==========================================================
    FILE *teste = fopen(nome_original, "w");
    if (teste) {
        for(int i=0; i<100; i++) {
            fprintf(teste, "A compressao de Huffman funciona incrivelmente bem para arquivos grandes que possuem muitas repeticoes de letras! ");
        }
        fclose(teste);
    }

    // ==========================================================
    // PASSO 1: Mapear o Arquivo (Contar frequências)
    // ==========================================================
    int frequencias[256];
    if (!calcularFrequencias(nome_original, frequencias)) {
        return 1; // Falhou ao ler o arquivo
    }

    // ==========================================================
    // PASSO 2: Construir a Árvore e o Dicionário
    // ==========================================================
    char **dicionario = construirDicionario(frequencias);
    if (dicionario == NULL) {
        printf("Arquivo vazio ou erro ao construir dicionario.\n");
        return 1;
    }

    // (Opcional) Visualizar alguns códigos gerados
    printf("--- Alguns codigos gerados ---\n");
    if(dicionario['A'] != NULL) printf("Letra 'A': %s\n", dicionario['A']);
    if(dicionario['a'] != NULL) printf("Letra 'a': %s\n", dicionario['a']);
    if(dicionario[' '] != NULL) printf("Espaco   : %s\n", dicionario[' ']);
    printf("------------------------------\n");

    // ==========================================================
    // PASSO 3: Comprimir de Fato!
    // ==========================================================
    printf("Comprimindo arquivo...\n");
    if (comprimirArquivo(nome_original, nome_comprimido, dicionario, frequencias)) {
        printf("Sucesso! Arquivo comprimido gerado: %s\n", nome_comprimido);
    }

    // ==========================================================
    // PASSO 4: Limpeza de Memória
    // ==========================================================
    for (int i = 0; i < 256; i++) {
        if (dicionario[i] != NULL) {
            free(dicionario[i]);
        }
    }
    free(dicionario);

    return 0;
}