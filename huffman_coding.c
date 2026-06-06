#include "huffman.h"

int main() {
    // 1. Dicionário inicial
    char componentes[] = "abr";
    int freq[] = {2, 1, 1};
    int n = strlen(componentes);

    int ans_size = 0;
    char** codigos = huffmanCodes(componentes, freq, n, &ans_size);

    printf("--- Dicionario Huffman ---\n");
    for(int i = 0; i < ans_size; i++) {
        printf("Letra '%c' -> Codigo: %s\n", componentes[i], codigos[i]);
    }
    printf("--------------------------\n\n");

    // 2. String alvo
    char string_alvo[] = "abra";
    printf("Texto Original     : %s\n", string_alvo);

    // 3. Codificando
    char* resultado_binario = codificarString(string_alvo, componentes, codigos, n);
    printf("Texto Codificado   : %s\n", resultado_binario);

    // 4. Decodificando
    char* resultado_decodificado = decodificarString(resultado_binario, componentes, codigos, n);
    printf("Texto Decodificado : %s\n", resultado_decodificado);

    // 5. Limpeza de Memória
    free(resultado_binario);
    free(resultado_decodificado); // Nova liberação!
    for (int i = 0; i < ans_size; i++) {
        free(codigos[i]);
    }
    free(codigos);
}