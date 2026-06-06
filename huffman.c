#include "huffman.h"

static par make_par(int dado, int cont) {
    return (par){dado, cont};
}

void preOrder(No root, char** ans, char* curr, int depth, int* ans_idx) {
    if (root == NULL) return;

    // Se for um nó folha (representa um caractere)
    if (root->esq == NULL && root->dir == NULL) {
        // Caso de caractere único
        if (depth == 0) {
            curr[0] = '0';
            depth = 1;
        }

        curr[depth] = '\0'; // Finaliza a string atual

        // Aloca espaço para a string final e salva no vetor de respostas
        ans[*ans_idx] = (char*)malloc((depth + 1) * sizeof(char));
        strcpy(ans[*ans_idx], curr);
        (*ans_idx)++;
        return;
    }

    // Navega para o lado esquerdo (adiciona '0')
    curr[depth] = '0';
    preOrder(root->esq, ans, curr, depth + 1, ans_idx);

    // Navega para o lado direito (adiciona '1')
    curr[depth] = '1';
    preOrder(root->dir, ans, curr, depth + 1, ans_idx);
}

char** huffmanCodes(char* s, int* freq, int n, int* returnSize) {
    Heap h = createMinHeap(n);
    for (int i = 0; i < n; i++) {
        No tmp = create_no(make_par(s[i], freq[i]), i);
        insertKey(h, tmp);
    }

    if (n == 1) {
        char** ans = (char**)malloc(sizeof(char*));
        ans[0] = (char*)malloc(2 * sizeof(char));
        strcpy(ans[0], "0");
        *returnSize = 1;

        free(h->harr);
        free(h);
        return ans;
    }

    while (h->heap_size >= 2) {
        No l = extractMin(h);
        No r = extractMin(h);

        No newNode = create_internal(l, r);
        insertKey(h, newNode);
    }

    // Raiz da Huffman Tree
    No root = extractMin(h);

    char** ans = (char**)malloc(n * sizeof(char*));
    char* curr = (char*)malloc((n + 1) * sizeof(char)); // O pior caso de profundidade é n
    int ans_idx = 0;

    preOrder(root, ans, curr, 0, &ans_idx);

    // Define o tamanho retornado para iterarmos depois
    *returnSize = ans_idx;

    free_rec(root);
    free(curr);
    free(h->harr);
    free(h);

    return ans;
}

static int acharIndiceCaractere(char c, char* mapeamento, int tamanho_mapeamento) {
    for (int i = 0; i < tamanho_mapeamento; i++) {
        if (mapeamento[i] == c) {
            return i;
        }
    }
    return -1; // Caractere não encontrado
}

// Função que transforma a string original na sua versão codificada em bits
char* codificarString(char* texto_original, char* caracteres_unicos, char** codigos_huffman, int tamanho_mapeamento) {
    int tamanho_texto = strlen(texto_original);
    int tamanho_total_bits = 0;

    // Cálculo do tamanho da "string de bits"
    for (int i = 0; i < tamanho_texto; i++) {
        int idx = acharIndiceCaractere(texto_original[i], caracteres_unicos, tamanho_mapeamento);
        if (idx != -1) {
            tamanho_total_bits += strlen(codigos_huffman[idx]);
        }
    }

    char* string_codificada = (char*)malloc((tamanho_total_bits + 1) * sizeof(char));
    if (string_codificada == NULL) return NULL;

    // Inicializa a string como vazia pra poder usar o strcat
    string_codificada[0] = '\0';

    // Concatenar os códigos binários de cada caractere
    for (int i = 0; i < tamanho_texto; i++) {
        int idx = acharIndiceCaractere(texto_original[i], caracteres_unicos, tamanho_mapeamento);
        if (idx != -1) {
            strcat(string_codificada, codigos_huffman[idx]);
        }
    }

    return string_codificada;
}

// Transforma a string binária de volta para o texto original usando o dicionário
char* decodificarString(char* texto_codificado, char* caracteres_unicos, char** codigos_huffman, int tamanho_mapeamento) {
    int tamanho_codificado = strlen(texto_codificado);

    // O pior caso para o texto decodificado é ter o mesmo tamanho do codificado
    char* texto_decodificado = (char*)malloc((tamanho_codificado + 1) * sizeof(char));
    int decod_idx = 0;

    // Buffer temporário para acumular os bits ('0's e '1's) lidos
    char* buffer = (char*)malloc((tamanho_codificado + 1) * sizeof(char));
    int buf_idx = 0;

    for (int i = 0; i < tamanho_codificado; i++) {
        buffer[buf_idx] = texto_codificado[i];
        buf_idx++;
        buffer[buf_idx] = '\0'; // Mantém a string formatada para a comparação com strcmp

        // Verifica se o acúmulo atual bate com algum código do dicionário
        for (int j = 0; j < tamanho_mapeamento; j++) {
            if (strcmp(buffer, codigos_huffman[j]) == 0) {
                // Código encontrado! Salva a letra original.
                texto_decodificado[decod_idx] = caracteres_unicos[j];
                decod_idx++;

                // Reseta o buffer para começar a formar a próxima letra
                buf_idx = 0;
                buffer[0] = '\0';
                break; // Sai do loop interno e vai para o próximo bit
            }
        }
    }

    // Finaliza a string decodificada e limpa a memória do buffer
    texto_decodificado[decod_idx] = '\0';
    free(buffer);

    return texto_decodificado;
}