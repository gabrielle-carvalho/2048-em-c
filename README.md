# Jogo 2048 em C

Este é um projeto desenvolvido em linguagem C que implementa o clássico jogo 2048 para ser jogado diretamente no terminal. O jogo utiliza a biblioteca `ncurses` para renderização no terminal e oferece uma experiência simples e intuitiva.

---

## **Como Rodar**

### **Pré-requisitos**
- Sistema operacional com suporte a GCC.
- Biblioteca `ncurses` instalada.
  - No Linux, você pode instalar a biblioteca usando:
    ```bash
    sudo apt-get install libncurses5-dev libncursesw5-dev
    ```

---

### **Compilação**
1. Abra o terminal e navegue até o diretório onde o arquivo `2048.c` está localizado.
2. Compile o programa usando o comando:
    ```bash
    gcc -o 2048 2048.c -lncurses
    ```

---

### **Execução**
1. Após a compilação, execute o jogo com o comando:
    ```bash
    ./2048
    ```

---

## **Controles**
Durante o jogo, um menu com as seguintes opções será exibido no terminal:

### **Descrição dos Controles**
- **W**: Move os blocos para cima.
- **S**: Move os blocos para baixo.
- **A**: Move os blocos para a esquerda.
- **D**: Move os blocos para a direita.
- **Z**: Desfaz o último movimento.
- **B**: Salva o estado atual do jogo manualmente.
- **Q**: Sai do jogo.
- **H**: Exibe a ajuda com as instruções.

---

## **Recursos do Jogo**
- Sistema de pontuação.
- Funcionalidade de "desfazer" movimentos (undo).
- Salvamento automático ao sair do jogo:
  - O estado do tabuleiro é salvo ao sair com 'q' e carregado automaticamente na próxima execução.
  - O arquivo de salvamento é apagado caso o jogo salvo chegue ao fim.
 

---

Divirta-se jogando e explorando o código! 🎮
