#include "comprime.h"
#include "huffman.h"

GerenciadorDeBits* criarGerenciador(FILE *arq) {
    GerenciadorDeBits *gerenciador = (GerenciadorDeBits*)malloc(sizeof(GerenciadorDeBits));
    gerenciador->arquivo = arq;
    gerenciador->byte = 0;
    gerenciador->contagem_bits = 0;
    return gerenciador;
}

// COMPRESSÃO

void escreverCodigo(GerenciadorDeBits *escritor, const char *codigo_huffman) {
    int i = 0;

    // Percorre cada '0' ou '1' da string
    while (codigo_huffman[i] != '\0') {
        // Desloca os bits atuais uma casa para a esquerda para abrir espaço
        escritor->byte = escritor->byte << 1;

        // Se o caractere for '1', nós ligamos o último bit usando OR (|)
        if (codigo_huffman[i] == '1') {
            escritor->byte = escritor->byte | 1;
        }
        // Se for '0', não precisamos fazer nada pois o bit novo já é 0 por causa do deslocamento

        escritor->contagem_bits++;

        // Se enchemos o byte (8 bits), escrevemos no arquivo e resetamos
        if (escritor->contagem_bits == 8) {
            fwrite(&(escritor->byte), sizeof(uint8_t), 1, escritor->arquivo);
            escritor->byte = 0;
            escritor->contagem_bits = 0;
        }

        i++;
    }
}

void finalizarEscritor(GerenciadorDeBits *escritor) {
    // Se ainda tem bits aguardando para serem escritos
    if (escritor->contagem_bits > 0) {
        // Precisamos empurrar esses bits para a esquerda para que fiquem
        // na parte mais significativa do byte.
        // Ex: se temos 3 bits "101", eles estão no final (00000101).
        // Empurramos 5 casas para virar (10100000).
        int bits_faltantes = 8 - escritor->contagem_bits;
        escritor->byte = escritor->byte << bits_faltantes;

        // Grava o último byte incompleto
        fwrite(&(escritor->byte), sizeof(uint8_t), 1, escritor->arquivo);
    }

    // Libera a memória da estrutura
    free(escritor);
}

int calcularFrequencias(const char *nome_arquivo, int *frequencias) {
    // Zera todas as 256 posições primeiro
    for (int i = 0; i < DICT_SIZE; i++) {
        frequencias[i] = 0;
    }

    // Abrimos em "rb" (read binary).
    // É crucial usar "rb" para que o SO não altere os bytes de quebra de linha internamente.
    FILE *arq = fopen(nome_arquivo, "rb");
    if (arq == NULL) {
        printf("ERRO: Nao foi possivel abrir o arquivo '%s' para leitura.\n", nome_arquivo);
        return 0;
    }

    uint8_t byte;
    while (fread(&byte, sizeof(uint8_t), 1, arq) == 1) {
        frequencias[byte]++;
    }

    fclose(arq);
    return 1;
}

static par make_par(int dado, int cont) {
    return (par){dado, cont};
}

// Função preOrder para mapeamento direto na tabela ASCII/Byte
void preOrder_comp(No root, char** ans, char* curr, int depth) {
    if (root == NULL) return;

    // Nó folha: encontramos um byte!
    if (root->esq == NULL && root->dir == NULL) {
        if (depth == 0) {
            curr[0] = '0';
            depth = 1;
        }

        curr[depth] = '\0';

        // Em vez de usar um contador sequencial, usamos o próprio valor do byte
        // garantindo que ele caia na posição certa (0 a 255)
        int byte_valor = root->dados.dado;
        ans[byte_valor] = (char*)malloc((depth + 1) * sizeof(char));
        strcpy(ans[byte_valor], curr);

        return;
    }

    curr[depth] = '0';
    preOrder_comp(root->esq, ans, curr, depth + 1);

    curr[depth] = '1';
    preOrder_comp(root->dir, ans, curr, depth + 1);
}

char** construirDicionario(int *frequencias) {
    int bytes_unicos = 0;
    for (int i = 0; i < DICT_SIZE; i++) {
        if (frequencias[i] > 0) bytes_unicos++;
    }

    if (bytes_unicos == 0) return NULL;

    Heap h = createMinHeap(bytes_unicos);

    // Insere na Heap
    for (int i = 0; i < DICT_SIZE; i++) {
        if (frequencias[i] > 0) {
            No tmp = create_no(make_par(i, frequencias[i]), i);
            insertKey(h, tmp);
        }
    }

    // Tratamento para caso o arquivo tenha apenas 1 tipo de byte repetido
    if (bytes_unicos == 1) {
        char** ans = (char**)calloc(DICT_SIZE, sizeof(char*));
        No root = extractMin(h);

        ans[root->dados.dado] = (char*)malloc(2 * sizeof(char));
        strcpy(ans[root->dados.dado], "0");

        free(root);
        free(h->harr);
        free(h);
        return ans;
    }

    // Constrói a Árvore
    while (h->heap_size >= 2) {
        No l = extractMin(h);
        No r = extractMin(h);
        No newNode = create_internal(l, r);
        insertKey(h, newNode);
    }

    No root = extractMin(h);

    char** ans = (char**)calloc(DICT_SIZE, sizeof(char*));
    char* curr = (char*)malloc((DICT_SIZE + 1) * sizeof(char)); // Pior caso de profundidade é 256 + 1 (\0)

    preOrder_comp(root, ans, curr, 0);

    // Limpeza
    free(curr);
    free(h->harr);
    free(h);
    free_rec(root);

    return ans;
}

int comprimirArquivo(const char *arquivo_entrada, const char *arquivo_saida, char **dicionario, int *frequencias) {
    FILE *entrada = fopen(arquivo_entrada, "rb");
    if (entrada == NULL) {
        printf("ERRO: Nao foi possivel abrir o arquivo de entrada.\n");
        return 0;
    }

    FILE *saida = fopen(arquivo_saida, "wb");
    if (saida == NULL) {
        printf("ERRO: Nao foi possivel criar o arquivo de saida.\n");
        fclose(entrada);
        return 0;
    }

    // Cabeçalho (Metadados)
    fwrite(frequencias, sizeof(int), 256, saida);

    // Compressão
    GerenciadorDeBits *escritor = criarGerenciador(saida);
    uint8_t byte;

    // Lê o arquivo original byte por byte
    while (fread(&byte, sizeof(uint8_t), 1, entrada) == 1) {
        // Busca o código Huffman correspondente ao byte lido
        char *codigo_bits = dicionario[byte];

        // Se o arquivo original não corrompeu desde a leitura das frequências, o código existirá
        if (codigo_bits != NULL) {
            escreverCodigo(escritor, codigo_bits);
        }
    }

    // Força a escrita de qualquer bit que ficou "preso" no último byte incompleto
    finalizarEscritor(escritor);

    fclose(entrada);
    fclose(saida);

    return 1;
}

// DESCOMPRESSÃO

int lerBit(GerenciadorDeBits *gerenciador) {
    // Se esgotamos os bits do buffer atual, lemos o próximo byte do disco
    if (gerenciador->contagem_bits == 0) {
        if (fread(&(gerenciador->byte), sizeof(uint8_t), 1, gerenciador->arquivo) != 1) {
            return -1; // Fim do arquivo (EOF)
        }
        gerenciador->contagem_bits = 8;
    }

    // Extrai o bit mais à esquerda
    int bit = (gerenciador->byte >> 7) & 1;

    // Empurra o byte 1 casa para a esquerda, preparando o próximo bit
    gerenciador->byte = gerenciador->byte << 1;

    gerenciador->contagem_bits--;

    return bit;
}

No construirArvore(int *frequencias) {
    int bytes_unicos = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0) bytes_unicos++;
    }

    if (bytes_unicos == 0) return NULL;

    Heap h = createMinHeap(bytes_unicos);
    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0) {
            No tmp = create_no(make_par(i, frequencias[i]), i);
            insertKey(h, tmp);
        }
    }

    // Se o arquivo original só tinha 1 caractere repetido várias vezes
    if (bytes_unicos == 1) {
        No root = extractMin(h);
        // Criamos um nó falso acima dele para permitir que a descida pela esquerda ('0') funcione
        No dummy = create_no(make_par(-1, root->dados.cont), -1);
        dummy->esq = root;
        free(h->harr); free(h);
        return dummy;
    }

    // Constrói a Árvore de Huffman unindo os menores
    while (h->heap_size >= 2) {
        No l = extractMin(h);
        No r = extractMin(h);
        No newNode = create_internal(l, r);
        insertKey(h, newNode);
    }

    No root = extractMin(h);

    free(h->harr);
    free(h);
    return root;
}

int descomprimirArquivo(const char *arquivo_comprimido, const char *arquivo_saida) {
    FILE *entrada = fopen(arquivo_comprimido, "rb");
    if (entrada == NULL) return 0;

    FILE *saida = fopen(arquivo_saida, "wb");
    if (saida == NULL) {
        fclose(entrada);
        return 0;
    }

    // Leitura do cabeçalho
    int frequencias[256];
    if (fread(frequencias, sizeof(int), 256, entrada) != 256) {
        printf("ERRO: Arquivo corrompido.\n");
        return 0;
    }

    // Cálculo dos bytes
    int total_bytes_originais = 0;
    for (int i = 0; i < 256; i++) {
        total_bytes_originais += frequencias[i];
    }

    // Reconstrução da árvore
    No root = construirArvore(frequencias);
    if (root == NULL) return 0;

    GerenciadorDeBits *gerenciador = criarGerenciador(entrada);
    No atual = root;
    int bytes_decodificados = 0;

    // Navega pela árvore bit a bit
    while (bytes_decodificados < total_bytes_originais) {
        int bit = lerBit(gerenciador);
        if (bit == -1) break;

        // Desce na árvore
        if (bit == 0) {
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }

        // Se chegamos em um nó folha (um byte original)
        if (atual->esq == NULL && atual->dir == NULL) {
            // Extrai o byte original
            uint8_t byte_original = atual->dados.dado;

            // Grava no arquivo
            fwrite(&byte_original, sizeof(uint8_t), 1, saida);

            // Volta para a raiz da árvore para começar a decifrar a próxima letra
            atual = root;

            bytes_decodificados++;
        }
    }

    free(gerenciador);
    fclose(entrada);
    fclose(saida);
    free_rec(root);

    return 1;
}