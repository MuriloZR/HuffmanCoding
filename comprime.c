#include "comprime.h"
#include <string.h>

GerenciadorDeBits* criarGerenciador(FILE *arq) {
    GerenciadorDeBits *gerenciador = (GerenciadorDeBits*)malloc(sizeof(GerenciadorDeBits));
    gerenciador->arquivo = arq;
    gerenciador->byte = 0;
    gerenciador->contagem_bits = 0;
    return gerenciador;
}

int64_t obterTamanhoArquivo(const char *nome_arquivo) {
    FILE *arq = fopen(nome_arquivo, "rb");
    if (arq == NULL) return -1;

    // Se estiver no Windows, usa as funções de 64 bits da Microsoft
    // Se estiver no Linux/Mac, usa as funções padrão POSIX de 64 bits
#ifdef _WIN32
    _fseeki64(arq, 0, SEEK_END);
    int64_t tamanho = _ftelli64(arq);
#else
    fseeko(arq, 0, SEEK_END);
    int64_t tamanho = ftello(arq);
#endif

    fclose(arq);
    return tamanho;
}

// COMPRESSÃO

void escreverCodigo(GerenciadorDeBits *escritor, unsigned int codigo, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        // Extrai o bit da posição mais significativa do código até a menos significativa
        int bit = (codigo >> (tamanho - 1 - i)) & 1;

        // Desloca o byte atual e insere o bit extraído
        escritor->byte = (escritor->byte << 1) | bit;
        escritor->contagem_bits++;

        // Se enchemos o byte (8 bits), escrevemos no disco
        if (escritor->contagem_bits == 8) {
            fwrite(&(escritor->byte), sizeof(uint8_t), 1, escritor->arquivo);
            escritor->byte = 0;
            escritor->contagem_bits = 0;
        }
    }
}

void finalizarEscritor(GerenciadorDeBits *escritor) {
    if (escritor->contagem_bits > 0) {
        int bits_faltantes = 8 - escritor->contagem_bits;
        escritor->byte = escritor->byte << bits_faltantes;
        fwrite(&(escritor->byte), sizeof(uint8_t), 1, escritor->arquivo);
    }
    free(escritor);
}

int calcularFrequencias(const char *nome_arquivo, uint64_t *frequencias) {
    for (int i = 0; i < DICT_SIZE; i++) frequencias[i] = 0;

    FILE *arq = fopen(nome_arquivo, "rb");
    if (arq == NULL) return 0;

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

void preencherDicionario(No root, unsigned int codigo_atual, int depth, CodigoHuffman* dict) {
    if (root == NULL) return;

    if (root->esq == NULL && root->dir == NULL) {
        // Tratamento para nó único (raiz)
        if (depth == 0) {
            codigo_atual = 0;
            depth = 1;
        }

        // Salva diretamente na Struct!
        dict[root->dados.dado].bits = codigo_atual;
        dict[root->dados.dado].tamanho = depth;
        return;
    }

    // Esquerda: Apenas empurra o valor (adiciona um bit 0 no final)
    preencherDicionario(root->esq, codigo_atual << 1, depth + 1, dict);

    // Direita: Empurra e faz um OR com 1 (adiciona um bit 1 no final)
    preencherDicionario(root->dir, (codigo_atual << 1) | 1, depth + 1, dict);
}

CodigoHuffman* construirDicionario(uint64_t *frequencias) {
    int bytes_unicos = 0;
    for (int i = 0; i < DICT_SIZE; i++) {
        if (frequencias[i] > 0) bytes_unicos++;
    }

    if (bytes_unicos == 0) return NULL;

    Heap h = createMinHeap(bytes_unicos);
    for (int i = 0; i < DICT_SIZE; i++) {
        if (frequencias[i] > 0) {
            No tmp = create_no(make_par(i, frequencias[i]), i);
            insertKey(h, tmp);
        }
    }

    if (bytes_unicos == 1) {
        CodigoHuffman* ans = (CodigoHuffman*)calloc(DICT_SIZE, sizeof(CodigoHuffman));
        No root = extractMin(h);
        ans[root->dados.dado].bits = 0;
        ans[root->dados.dado].tamanho = 1;
        free(root); free(h->harr); free(h);
        return ans;
    }

    while (h->heap_size >= 2) {
        No l = extractMin(h);
        No r = extractMin(h);
        No newNode = create_internal(l, r);
        insertKey(h, newNode);
    }

    No root = extractMin(h);

    CodigoHuffman* ans = (CodigoHuffman*)calloc(DICT_SIZE, sizeof(CodigoHuffman));

    // Inicia a recursão com código 0 e profundidade 0
    preencherDicionario(root, 0, 0, ans);

    free(h->harr);
    free(h);
    free_rec(root);

    return ans;
}

int comprimirArquivo(const char *arquivo_entrada, const char *arquivo_saida, CodigoHuffman *dicionario, uint64_t *frequencias) {
    FILE *entrada = fopen(arquivo_entrada, "rb");
    if (entrada == NULL) return 0;

    FILE *saida = fopen(arquivo_saida, "wb");
    if (saida == NULL) { fclose(entrada); return 0; }

    fwrite(frequencias, sizeof(uint64_t), 256, saida);
    GerenciadorDeBits *escritor = criarGerenciador(saida);
    uint8_t byte;

    while (fread(&byte, sizeof(uint8_t), 1, entrada) == 1) {
        // Se o tamanho for maior que 0, o caractere existe no dicionário
        if (dicionario[byte].tamanho > 0) {
            escreverCodigo(escritor, dicionario[byte].bits, dicionario[byte].tamanho);
        }
    }

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

No construirArvore(uint64_t *frequencias) {
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
    uint64_t frequencias[256];
    if (fread(frequencias, sizeof(uint64_t), 256, entrada) != 256) {
        printf("ERRO: Arquivo corrompido.\n");
        return 0;
    }

    // Cálculo dos bytes
    uint64_t total_bytes_originais = 0;
    for (int i = 0; i < 256; i++) {
        total_bytes_originais += frequencias[i];
    }

    // Reconstrução da árvore
    No root = construirArvore(frequencias);
    if (root == NULL) return 0;

    GerenciadorDeBits *gerenciador = criarGerenciador(entrada);
    No atual = root;
    uint64_t bytes_decodificados = 0;

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