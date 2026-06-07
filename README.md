# Compressor de Arquivos - Algoritmo de Huffman

Um utilitário de linha de comando (CLI) escrito totalmente em **C** para compressão e descompressão de arquivos sem perdas (lossless) utilizando a estrutura de dados da **Árvore de Huffman**. 

Diferente de implementações acadêmicas que salvam os códigos binários como strings (o que na verdade aumenta o tamanho do arquivo), este projeto manipula a memória a nível de bits (Bitwise) para escrever e ler bits individuais no disco, alcançando compressão real.

---

## Sobre o Projeto

O objetivo deste projeto é demonstrar a aplicação prática de Estruturas de Dados Avançadas e manipulação de arquivos binários em C. O algoritmo mapeia a frequência de cada byte (0 a 255) em um arquivo, constrói uma Fila de Prioridade (Min-Heap) e gera uma Árvore de Huffman para criar prefixos binários otimizados. Bytes muito frequentes recebem códigos curtos (ex: `10`), enquanto bytes raros recebem códigos mais longos.

## Funcionalidades

* **Compressão Real:** Reduz o tamanho de arquivos de texto preservando 100% da integridade original.
* **Manipulação Bit a Bit:** Gravação e leitura de arquivos `.huff` utilizando operações Bitwise (`<<`, `>>`, `&`, `|`).
* **Cabeçalho Customizado:** O arquivo comprimido carrega sua própria tabela de frequências para permitir a reconstrução da árvore em qualquer máquina.
* **Modo Demonstração:** Permite visualizar a árvore gerada e a codificação de uma string diretamente no terminal, ideal para fins educacionais.

---

## Como Compilar e Usar

**Pré-requisitos:** É necessário ter o compilador GCC instalado na sua máquina.

### Compilação
Clone o repositório e compile o arquivo fonte:
```bash
gcc heap.c huffman.c comprime.c main.c -o huffman

```

---

### Comprimir um Arquivo (`-c`)

Gera um arquivo `.huff` contendo o cabeçalho e os bits comprimidos.

```bash
./huffman -c arquivo_original.txt arquivo_comprimido.huff

```

---

### Descomprimir um Arquivo (`-d`)

Lê um arquivo `.huff`, reconstrói a árvore a partir do cabeçalho e extrai o arquivo original bit a bit.

```bash
./huffman -d arquivo_comprimido.huff arquivo_restaurado.txt

```

---

### Modo de Demonstração (`-demo`)

Insira uma frase e veja o dicionário gerado e o resultado binário sem precisar criar arquivos.

```bash
./huffman -demo "abracadabra"

```

---

## Estruturas de Dados Utilizadas

* **Array de Frequências:** Mapeamento de O(1) para a contagem de ocorrências de cada byte (0 a 255).
* **Min-Heap (Fila de Prioridade):** Utilizada para ordenar e extrair os nós de menor frequência com eficiência O(log N) durante a construção da árvore.
* **Árvore Binária (Huffman Tree):** Estrutura ramificada onde folhas representam os bytes originais e os caminhos (esquerda/direita) representam os bits `0` e `1`.
* **Bit Buffer (Acumulador):** Variável de 8 bits que agrupa os `0`s e `1`s individuais antes de realizar a gravação no disco (`fwrite`).

---
