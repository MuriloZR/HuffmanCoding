#include "comprime.h"
#include "huffman.h"

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

        // Calcula frequencias globais (0 a 255)
        int frequencias[256] = {0};
        for (int i = 0; texto[i] != '\0'; i++) {
            frequencias[(unsigned char)texto[i]]++;
        }

        // Prepara os arrays compactos exigidos pela função huffmanCodes
        int n = 0;
        for (int i = 0; i < 256; i++) {
            if (frequencias[i] > 0) n++;
        }

        char *caracteres_unicos = (char*)malloc(n * sizeof(char));
        int *freq_compacta = (int*)malloc(n * sizeof(int));

        int idx = 0;
        for (int i = 0; i < 256; i++) {
            if (frequencias[i] > 0) {
                caracteres_unicos[idx] = (char)i;
                freq_compacta[idx] = frequencias[i];
                idx++;
            }
        }

        int returnSize = 0;
        char **codigos_huffman = huffmanCodes(caracteres_unicos, freq_compacta, n, &returnSize);

        // Printa o Dicionário Gerado
        printf("\n--- Dicionario de Codigos ---\n");
        for (int i = 0; i < returnSize; i++) {
            if (caracteres_unicos[i] == ' ')
                printf("Caractere [espaco] : %s\n", codigos_huffman[i]);
            else
                printf("Caractere '%c'      : %s\n", caracteres_unicos[i], codigos_huffman[i]);
        }

        printf("\n--- Resultado Codificado ---\n");
        char *resultado_bits = codificarString(texto, caracteres_unicos, codigos_huffman, returnSize);
        if (resultado_bits != NULL) {
            printf("%s\n\n", resultado_bits);
            free(resultado_bits);
        }

        for (int i = 0; i < returnSize; i++) {
            free(codigos_huffman[i]);
        }
        free(codigos_huffman);
        free(caracteres_unicos);
        free(freq_compacta);

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

        uint64_t frequencias[256];
        if (!calcularFrequencias(arquivo_origem, frequencias)) return 1;

        CodigoHuffman *dicionario = construirDicionario(frequencias);
        if (dicionario == NULL) {
            printf("ERRO: O arquivo origem esta vazio ou houve falha.\n");
            return 1;
        }

        if (comprimirArquivo(arquivo_origem, arquivo_destino, dicionario, frequencias)) {
            // Pega o tamanho dos dois arquivos
            uint64_t tam_original = obterTamanhoArquivo(arquivo_origem);
            uint64_t tam_comprimido = obterTamanhoArquivo(arquivo_destino);

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