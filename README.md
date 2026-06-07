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

## Como o Algoritmo Funciona (Fluxo Lógico)

O processo de compressão e descompressão segue uma arquitetura rigorosa de passos para garantir que nenhum dado seja perdido.

### 📉 O Processo de Compressão

1. **Mapeamento de Frequências:** O arquivo original é lido do início ao fim, byte a byte. O algoritmo utiliza um array de 256 posições (representando todos os valores possíveis de um byte, de 0 a 255) para contar quantas vezes cada byte aparece.
2. **Fila de Prioridade (Min-Heap):** Cada byte que apareceu no arquivo é transformado em um nó folha e inserido em uma Min-Heap, ordenado pela sua frequência (bytes mais raros ficam no topo).
3. **Construção da Árvore de Huffman:** O algoritmo remove os dois nós de menor frequência da Heap e cria um nó "pai" unindo-os. A frequência do pai é a soma das frequências dos filhos. Esse pai volta para a Heap. O processo se repete até sobrar apenas um nó: a raiz da árvore.
4. **Geração do Dicionário Binário:** A árvore é percorrida da raiz às folhas. Navegar para a esquerda adiciona um bit `0`, para a direita um bit `1`. Quando uma folha é alcançada, o caminho percorrido se torna o "código" daquele byte. Bytes frequentes acabam no topo da árvore (códigos curtos), enquanto os raros ficam no fundo (códigos longos).
5. **Gravação do Arquivo (.huff):**
   * **Cabeçalho:** As 256 frequências são gravadas no início do arquivo de saída.
   * **Corpo:** O arquivo original é lido novamente. Para cada byte lido, o algoritmo busca seu novo código em zeros e uns.
   * **Operações Bitwise:** Como o C não permite salvar "meio byte", usamos um *Buffer de 8 bits* e operadores de deslocamento (`<<`, `>>`, `|`) para empacotar os bits individuais. Quando o buffer enche, ele é gravado no disco como 1 byte real.

---

### 📈 O Processo de Descompressão

1. **Leitura do Cabeçalho:** O algoritmo lê os primeiros 256 inteiros do arquivo `.huff`. Isso permite saber exatamente a frequência original de cada byte e calcular a quantidade exata de bytes que o arquivo original possuía.
2. **Reconstrução da Árvore:** Usando o array de frequências lido no passo anterior, o algoritmo refaz os passos de montagem da Min-Heap e da Árvore de Huffman. O resultado é uma árvore matematicamente idêntica à que foi usada na compressão.
3. **Leitura Bit a Bit:** O restante do arquivo comprimido é lido. Usando máscaras de bits (`&`), extraímos os bits individualmente, um a um.
4. **Navegação e Extração:**
   * O algoritmo começa na raiz da árvore.
   * Se ler `0`, desce para o nó da esquerda. Se ler `1`, desce para a direita.
   * Quando atinge um nó folha (sem filhos), o byte original foi encontrado! O byte é gravado no arquivo de destino e o ponteiro volta para a raiz da árvore para iniciar a decodificação do próximo caractere.
   * Esse processo continua até que o total de bytes restaurados seja igual ao tamanho do arquivo original.

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
