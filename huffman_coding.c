#include "huffman.h"
#include "comprime.h"

int main() {
    const char *nome_original = "entrada.txt";
    const char *nome_comprimido = "saida.huff";
    const char *nome_restaurado = "restaurado.txt"; // Nosso arquivo final

    // 1. Cria o arquivo de teste original
    FILE *teste = fopen(nome_original, "w");
    if (teste) {
        for(int i=0; i<100; i++) {
            fprintf(teste, "A compressao de Huffman funciona incrivelmente bem para arquivos grandes que possuem muitas repeticoes de letras!\n");
        }
        fclose(teste);
    }

    // 2. Extrai Frequências e Comprime
    int frequencias[256];
    if (calcularFrequencias(nome_original, frequencias)) {
        char **dicionario = construirDicionario(frequencias);
        if (dicionario != NULL) {
            printf("Comprimindo...\n");
            if(comprimirArquivo(nome_original, nome_comprimido, dicionario, frequencias)) {
                printf("Compressao concluida com sucesso!\n");
            }
            // Limpa o dicionario
            for (int i = 0; i < 256; i++) {
                if (dicionario[i]) free(dicionario[i]);
            }
            free(dicionario);
        }
    }

    // 3. Descomprime (A mágica que acabamos de fazer)
    printf("Descomprimindo...\n");
    if (descomprimirArquivo(nome_comprimido, nome_restaurado)) {
        printf("Descompressao concluida! Abra '%s' e veja que esta perfeitamente igual ao original.\n", nome_restaurado);
    }

    return 0;
}