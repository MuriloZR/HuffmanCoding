#include "comprime.h"

// Retorna o tamanho do arquivo em bytes, ou -1 em caso de erro
long obterTamanhoArquivo(const char *nome_arquivo) {
    FILE *arq = fopen(nome_arquivo, "rb");
    if (arq == NULL) {
        return -1;
    }

    fseek(arq, 0, SEEK_END);

    long tamanho = ftell(arq);

    fclose(arq);
    return tamanho;
}

void imprimirAjuda(const char *nome_programa) {
    printf("==========================================\n");
    printf("         COMPRESSOR DE HUFFMAN            \n");
    printf("==========================================\n");
    printf("Uso: %s <flag> [argumentos...]\n\n", nome_programa);
    printf("Flags de Arquivo:\n");
    printf("  -c   Comprimir    : %s -c <origem.txt> <destino.huff>\n", nome_programa);
    printf("  -d   Descomprimir : %s -d <origem.huff> <destino.txt>\n\n", nome_programa);
    printf("Flag de Demonstracao:\n");
    printf("  -demo Mostrar arvore : %s -demo \"sua string aqui\"\n", nome_programa);
    printf("==========================================\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        imprimirAjuda(argv[0]);
        return 1;
    }

    char *flag = argv[1];

    // ==========================================
    // FLUXO DE DEMONSTRAÇÃO (-demo)
    // ==========================================
    if (strcmp(flag, "-demo") == 0) {
        if (argc != 3) {
            printf("ERRO: O modo -demo exige exatamente 1 string entre aspas.\n");
            printf("Exemplo: %s -demo \"abracadabra\"\n", argv[0]);
            return 1;
        }

        char *texto = argv[2];
        printf("\nTexto Original : '%s'\n", texto);

        // Calcula frequencias direto da string
        int frequencias[256] = {0};
        for (int i = 0; texto[i] != '\0'; i++) {
            // Converte para unsigned char para evitar índices negativos com acentos
            frequencias[(unsigned char)texto[i]]++;
        }

        // Constrói a árvore/dicionário
        char **dicionario = construirDicionario(frequencias);
        if (dicionario == NULL) {
            printf("Erro: String vazia.\n");
            return 1;
        }

        // Printa o Dicionário Gerado
        printf("\n--- Dicionario de Codigos ---\n");
        for (int i = 0; i < 256; i++) {
            if (frequencias[i] > 0 && dicionario[i] != NULL) {
                // Se for o caractere de espaço, printa "[espaco]" para ficar legível
                if (i == ' ')
                    printf("Caractere [espaco] : %s\n", dicionario[i]);
                else
                    printf("Caractere '%c'      : %s\n", i, dicionario[i]);
            }
        }

        // Printa a String Codificada
        printf("\n--- Resultado Codificado ---\n");
        for (int i = 0; texto[i] != '\0'; i++) {
            printf("%s", dicionario[(unsigned char)texto[i]]);
        }
        printf("\n\n");

        // Limpeza de memória
        for (int i = 0; i < 256; i++) {
            if (dicionario[i]) free(dicionario[i]);
        }
        free(dicionario);

        return 0;
    }

    // ==========================================
    // FLUXO DE COMPRESSÃO E DESCOMPRESSÃO
    // ==========================================
    // Caso a flag seja "-c" ou "-d"
    if (argc != 4) {
        imprimirAjuda(argv[0]);
        return 1;
    }

    char *arquivo_origem = argv[2];
    char *arquivo_destino = argv[3];

    if (strcmp(flag, "-c") == 0) {
        printf("Iniciando compressao de '%s' para '%s'...\n", arquivo_origem, arquivo_destino);

        int frequencias[256];
        if (!calcularFrequencias(arquivo_origem, frequencias)) return 1;

        char **dicionario = construirDicionario(frequencias);
        if (dicionario == NULL) {
            printf("ERRO: O arquivo origem esta vazio ou houve falha.\n");
            return 1;
        }

        if (comprimirArquivo(arquivo_origem, arquivo_destino, dicionario, frequencias)) {
            // Pega o tamanho dos dois arquivos
            long tam_original = obterTamanhoArquivo(arquivo_origem);
            long tam_comprimido = obterTamanhoArquivo(arquivo_destino);

            printf("\n>> Sucesso! Arquivo comprimido gerado.\n");

            // Verifica se conseguiu ler os tamanhos corretamente
            if (tam_original > 0 && tam_comprimido > 0) {
                // Matemática da redução: 100% - ( (comprimido / original) * 100 )
                double porcentagem_comprimido = ((double)tam_comprimido / tam_original) * 100.0;
                double reducao = 100.0 - porcentagem_comprimido;

                printf("Tamanho Original   : %ld bytes\n", tam_original);
                printf("Tamanho Comprimido : %ld bytes\n", tam_comprimido);

                // Se a redução for negativa (arquivo aumentou), avisamos o usuário
                if (reducao >= 0) {
                    printf("Reducao de Tamanho : %.2f%%\n", reducao);
                } else {
                    printf("Reducao de Tamanho : %.2f%% (O arquivo aumentou devido ao cabecalho)\n", reducao);
                }
            }
        }

        for (int i = 0; i < 256; i++) {
            if (dicionario[i]) free(dicionario[i]);
        }
        free(dicionario);

    } else if (strcmp(flag, "-d") == 0) {
        printf("Iniciando descompressao de '%s' para '%s'...\n", arquivo_origem, arquivo_destino);

        if (descomprimirArquivo(arquivo_origem, arquivo_destino)) {
            printf(">> Sucesso! Arquivo original restaurado.\n");
        } else {
            printf("ERRO: Falha durante a descompressao.\n");
        }
    } else {
        printf("ERRO: Flag '%s' nao reconhecida.\n", flag);
        imprimirAjuda(argv[0]);
        return 1;
    }

    return 0;
}