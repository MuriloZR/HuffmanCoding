#include "comprime.h"
#include "huffman.h"

// Inicia o escritor associado a um arquivo já aberto para escrita binária ("wb")
EscritorDeBits* criarEscritor(FILE *arq) {
    EscritorDeBits *escritor = (EscritorDeBits*)malloc(sizeof(EscritorDeBits));
    escritor->arquivo = arq;
    escritor->byte = 0;
    escritor->contagem_bits = 0;
    return escritor;
}

// Função que converte uma string "0101" em bits reais e escreve no arquivo
void escreverCodigo(EscritorDeBits *escritor, const char *codigo_huffman) {
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

// Grava qualquer bit que tenha sobrado no buffer e libera o escritor
void finalizarEscritor(EscritorDeBits *escritor) {
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

// Certifique-se de ter incluído <stdint.h> no topo do arquivo
// #include <stdint.h>

// Preenche um array de 256 posições com a frequência de cada byte no arquivo
int calcularFrequencias(const char *nome_arquivo, int *frequencias) {
    // Zera todas as 256 posições primeiro
    for (int i = 0; i < DICT_SIZE; i++) {
        frequencias[i] = 0;
    }

    // Abrimos em "rb" (read binary).
    // É crucial usar "rb" mesmo para .txt para que o SO não altere os bytes de quebra de linha internamente.
    FILE *arq = fopen(nome_arquivo, "rb");
    if (arq == NULL) {
        printf("ERRO: Nao foi possivel abrir o arquivo '%s' para leitura.\n", nome_arquivo);
        return 0; // Retorna 0 indicando falha
    }

    uint8_t byte;
    // fread retorna o número de blocos lidos. Se for 1, lemos um byte com sucesso.
    while (fread(&byte, sizeof(uint8_t), 1, arq) == 1) {
        // O próprio valor do byte (0 a 255) serve como índice do array!
        frequencias[byte]++;
    }

    fclose(arq);
    return 1; // Retorna 1 indicando sucesso
}

static par make_par(int dado, int cont) {
    return (par){dado, cont};
}

// Função preOrder atualizada para mapeamento direto na tabela ASCII/Byte
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

// Nova versão que recebe a tabela de 256 posições
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
        char** ans = (char**)calloc(DICT_SIZE, sizeof(char*)); // calloc zera tudo (preenche com NULL)
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

    // Aloca o array de 256 posições preenchido com NULLs
    char** ans = (char**)calloc(DICT_SIZE, sizeof(char*));
    char* curr = (char*)malloc((DICT_SIZE + 1) * sizeof(char)); // Pior caso de profundidade é 256 + 1 (\0)

    // Percorre a árvore
    preOrder_comp(root, ans, curr, 0);

    // Limpeza
    free(curr);
    free(h->harr);
    free(h);

    // Liberação da árvore (DICA: Em uma implementação completa,
    // você precisará de uma função recursiva 'freeTree(root)' para evitar vazamento de memória aqui)
    free_rec(root);

    return ans;
}