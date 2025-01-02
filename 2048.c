#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h> // Biblioteca para manipulação da interface gráfica com o terminal (teclado, cores, etc) 
#include <unistd.h> // Biblioteca para funções como sleep()

#define WIN_SCORE 2048 // Define o valor que o jogador deve atingir para vencer
#define tam 4 // Tamanho do tabuleiro do jogo (4x4)

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    int **board; // Tabuleiro atual do jogo
    int **prev_board; //retaura estado do tabuleiro no undo
    int score; // Pontuação atual
    int highscore; // Melhor pontuação
    int moves; // Número de movimentos realizados
    bool game_over; // Flag que indica se o jogo acabou
    bool backup_saved;  // Flag para verificar se o jogo foi salvo
    int backup_score; //restaura o score no undo
    int ultimo_valor;
    int linha;
    int coluna;
    int backup_moves; // restaura qtd de moves no undo
    int **backup_board; //recupera tabuleiro salvo
    bool winscore_reached; // flag para verificar se winscore foi atingido
} Game;

void init_board(Game *game), init_colors(), loop_jogo(Game *game) , quit_game(Game *game), free_game(Game *game), print_board(const Game *game), backup(Game *game), update_score(Game *game), save_highscore(const Game *game), add_random_number(Game *game), move_board(Game *game, Direction dir), save_game(Game *game, const char *filename), load_game(Game *game, const char *filename), load_highscore(int *highscore), print_fixed_menu();
int menu(), change_value(Game *game);
bool check_win(Game *game), check_full(const Game *game), can_move(const Game *game);;

void init_board(Game *game) { // Aloca memória para o board e os backups
    game->board = (int **) calloc(tam, sizeof(int *));
    game->prev_board = (int **) calloc(tam, sizeof(int *));
    game->backup_board = (int **) malloc(tam * sizeof(int *));
    if (!game->board || !game->prev_board || !game->backup_board) {
        perror("Falha na alocação de memória.");
        return;
    }

    for (int i = 0; i < tam; i++) { // Aloca memória para cada linha do board 
        game->board[i] = (int *) calloc(tam, sizeof(int));
        game->prev_board[i] = (int *) calloc(tam, sizeof(int));
        game->backup_board[i] = (int *) malloc(tam * sizeof(int));
        if (!game->board[i] || !game->prev_board[i] || !game->backup_board[i]) {
            perror("Falha na alocação de memória para uma linha.");
            return;
        }
    }
    // Inicializa variáveis do jogo
    game->score = 0;
    game->moves = 0;
    game->game_over = false;
    game->backup_saved = false;

    add_random_number(game); //adiciona 2 numeros aleatorios no comeco
    add_random_number(game);
}

void free_game(Game *game) { // Libera a memória alocada para o jogo
    for (int i = 0; i < tam; i++) { // Libera memória de cada linha do tabuleiro e de cada backup
        free(game->board[i]);
        free(game->prev_board[i]);
        free(game->backup_board[i]);
    }
    free(game->board);
    free(game->prev_board);
    free(game->backup_board);
}

void backup(Game *game) { //ARMAZENA SEMPRE O ESTADO DO JOGO PARA CASO HAJA UNDO
    for (int i = 0; i < tam; i++) {
        memcpy(game->backup_board[i], game->board[i], tam * sizeof(int));
    }
    game->backup_score = game->score; // Salva a pontuação
    game->backup_moves = game->moves; // Salva o número de movimentos
    game->backup_saved = true; // Marca que o backup foi feito
}

void undo_move(Game *game) {  // desfaz o último movimento, restaurando o estado anterior
    if (!game->backup_saved) {
        printw("Nenhuma jogada para desfazer!\n");
        return;
    }
    game->score = game->backup_score;
    game->moves = game->backup_moves; // restaura o número de movimentos

    for (int i = 0; i < tam; i++) {
        memcpy(game->board[i], game->backup_board[i], tam * sizeof(int));// Restaura o estado do jogo
    }
    game->backup_saved = false;  // reseta o estado de backup
    printw("Última jogada desfeita.\n");
}

bool check_full(const Game *game) {
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if (game->board[i][j] == 0) { //percorre todo array e ao encontrar espaco livre retorna 0
                return false; //nao ta cheio, entao é falso
            }
        }
    }
    //depois de verificar que o tabuleiro está cheio, a função chama can_move
    // verifica se ainda há movimentos possíveis
    return !can_move(game); // Retorna verdadeiro se não houver mais movimentos possíveis
}

void add_random_number(Game *game) {
    if (check_full(game) && !can_move(game)) {
        game->game_over = true;
        return; // Não há espaço para adicionar um novo número
    }
    int i, j;
    do {
        i = rand() % tam;
        j = rand() % tam;
    } while (game->board[i][j] != 0);

    game->board[i][j] = (rand() % 10 < 6) ? 2 : 4; // Escolhe aleatoriamente 2 ou 4
    game->ultimo_valor = game->board[i][j];
    game->linha = i;
    game->coluna = j;
}

void init_colors() {
    if (has_colors()) {
        start_color();   // habilita o suporte a cores

        init_color(COLOR_RED, 800, 0, 0);
        init_color(COLOR_GREEN, 0, 800, 0);
        init_color(COLOR_BLUE, 0, 0, 800);
        init_color(COLOR_YELLOW, 800, 800, 0);
        init_color(COLOR_CYAN, 0, 800, 800);
        init_color(COLOR_MAGENTA, 800, 0, 800);
        init_color(COLOR_WHITE, 800, 800, 800);
        init_color(10, 600, 300, 800); // Roxo
        init_color(11, 1000, 600, 1000); // Roxo claro
        init_color(12, 1000, 800, 100);  // Rosinha

        // Define o par de cores para cada número
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_BLACK, COLOR_CYAN);
        init_pair(3, COLOR_BLACK, COLOR_GREEN);
        init_pair(4, COLOR_BLACK, COLOR_YELLOW);
        init_pair(5, COLOR_BLACK, COLOR_BLUE);
        init_pair(6, COLOR_BLACK, COLOR_MAGENTA);
        init_pair(7, COLOR_BLACK, COLOR_RED);
        init_pair(8, COLOR_BLACK, 10);  // Roxo
        init_pair(9, COLOR_BLACK, 14);  // Amarelo
        init_pair(10, COLOR_BLACK, 11); // Roxo claro
        init_pair(11, COLOR_BLACK, 12); // Rosinha claro
    }
}

void print_board(const Game *game) {
    clear(); // limpa a tela antes de exibir o novo tabuleiro
    print_fixed_menu();  // Exibe o menu fixo sempre no topo
    printw("-----------------------------\n");

    for (int i = 0; i < tam; i++) {
        printw("|");
        for (int j = 0; j < tam; j++) {
            int num = game->board[i][j];
            if (num == 0) {
                attron(COLOR_PAIR(1));  // Ativa o uso do par de cores 1
                printw("        |");
                attroff(COLOR_PAIR(1));  // Desativa o uso do par de cores 1
            } else {
                int color_pair = 2; // Cor padrão para 2
                // Atribuindo cores específicas para cada número
                if (num == 4) color_pair = 3;
                else if (num == 8) color_pair = 4;
                else if (num == 16) color_pair = 5;
                else if (num == 32) color_pair = 6;
                else if (num == 64) color_pair = 7;
                else if (num == 128) color_pair = 8;
                else if (num == 256) color_pair = 9;
                else if (num == 512) color_pair = 10;
                else if (num == 1024) color_pair = 11;
                else if (num == 2048) color_pair = 12;
                else if (num == 4096) color_pair = 13;
                else if (num == 8192) color_pair = 14;

                attron(COLOR_PAIR(color_pair));
                printw("  %4d  |", num);
                attroff(COLOR_PAIR(color_pair));
            }
        }
        printw("\n");
        printw("-----------------------------\n");
    }
    refresh(); // atualiza a tela
}

void print_fixed_menu() {
    printw("      2048 GAME 🎮     \n");
    printw("Menu: [W] Move Up | [S] Move Down | [A] Move Left | [D] Move Right\n");
    printw("[Z] Undo | [B] Save | [Q] Quit | [H] Help\n");
}

void save_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erro ao salvar o jogo");
        return;
    }

    // Salva score, highscore, moves
    fwrite(&game->score, sizeof(int), 1, file);
    fwrite(&game->highscore, sizeof(int), 1, file);
    fwrite(&game->moves, sizeof(int), 1, file);

    // Salva as três matrizes de tabuleiro (board, prev_board, backup_board)
    for (int i = 0; i < tam; i++) {
        fwrite(game->board[i], sizeof(int), tam, file);
        fwrite(game->prev_board[i], sizeof(int), tam, file);
        fwrite(game->backup_board[i], sizeof(int), tam, file);
    }

    fclose(file);
}

void load_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", filename);
        return;
    }

    // Libera memória dos tabuleiros, caso já tenha sido alocada
    if (game->board) { 
        for (int i = 0; i < tam; i++) {
            free(game->board[i]);
            free(game->prev_board[i]);
            free(game->backup_board[i]);
        }
        free(game->board);
        free(game->prev_board);
        free(game->backup_board);
    }

    // Aloca memória para os tabuleiros
    game->board = (int **)malloc(tam * sizeof(int *));
    game->prev_board = (int **)malloc(tam * sizeof(int *));
    game->backup_board = (int **)malloc(tam * sizeof(int *));
    
    if (!game->board || !game->prev_board || !game->backup_board) {
        printf("Erro ao alocar memória para os tabuleiros.\n");
        fclose(file);
        return;
    }

    for (int i = 0; i < tam; i++) {
        game->board[i] = (int *)malloc(tam * sizeof(int));
        game->prev_board[i] = (int *)malloc(tam * sizeof(int));
        game->backup_board[i] = (int *)malloc(tam * sizeof(int));

        if (!game->board[i] || !game->prev_board[i] || !game->backup_board[i]) {
            printf("Erro ao alocar memória para as linhas dos tabuleiros.\n");
            fclose(file);
            return;
        }
    }

    // Lê os dados salvos do jogo
    fread(&game->score, sizeof(int), 1, file);
    fread(&game->highscore, sizeof(int), 1, file);
    fread(&game->moves, sizeof(int), 1, file);

    // Lê os três tabuleiros (board, prev_board, backup_board)
    for (int i = 0; i < tam; i++) {
        fread(game->board[i], sizeof(int), tam, file);
        fread(game->prev_board[i], sizeof(int), tam, file);
        fread(game->backup_board[i], sizeof(int), tam, file);
    }

    fclose(file);
}

int change_value(Game *game){
    if (game->ultimo_valor == 2 ){
        game->board[game->linha][game->coluna] = 4;
    }
    else{
        game->board[game->linha][game->coluna] = 2;
    }
}

void update_score(Game *game) {
    if (game->score > game->highscore) {
        game->highscore = game->score;
        save_highscore(game);  // Salva o novo highscore no arquivo
    }
}

void save_highscore(const Game *game) {
    FILE *file = fopen("highscore.txt", "w");
    if (file) {
        fwrite(&game->highscore, sizeof(int), 1, file);
        fclose(file);
    }
}

void load_highscore(int *highscore) {
    FILE *file = fopen("highscore.txt", "r");
    if (file) {
        fread(highscore, sizeof(int), 1, file);
        fclose(file);
    } else {
        *highscore = 0;
    }
}

void move_board(Game *game, Direction dir) {
    int i, j, k;
    bool merged[tam][tam] = {{false}};  // Controle de combinações: cada célula é um booleano que indica se a célula foi combinada em um movimento
    bool moved = false;  // flag para saber se houve movimento: pra add number

    // Verifica se há movimentos possíveis antes de tentar mover
    if (!can_move(game)) {
        //printw("Não é possível mover!\n");
        refresh();
        return;
    }

    backup(game); // Realiza o backup do estado do jogo, caso precise desfazer

switch (dir) {
    case UP:  // Direção para cima
        for (j = 0; j < tam; j++) {  // Percorre todas as colunas (de 0 a tam-1)
            for (i = 1; i < tam; i++) {  // Começa pela segunda linha (i=1), pois a linha 0 já está no topo
                if (game->board[i][j] != 0) {  // Se encontrar um número diferente de 0
                    k = i;  // Armazena a linha atual em k (linha onde a peça foi encontrada)
                    // Move a peça para cima, procurando por espaços vazios
                    while (k > 0 && game->board[k-1][j] == 0) {  // Enquanto houver uma célula vazia acima
                        game->board[k-1][j] = game->board[k][j];  // Move a peça para cima
                        game->board[k][j] = 0;  // Limpa a célula onde a peça estava
                        k--;  // Move o índice k para cima (linha acima)
                        moved = true;  // Marca que houve movimento
                    }
                    // Tenta combinar as peças (mesmo valor e não combinadas ainda)
                    if (k > 0 && game->board[k-1][j] == game->board[k][j] && !merged[k-1][j] && !merged[k][j]) {
                        game->board[k-1][j] *= 2;  // Combina as peças (multiplica por 2)
                        game->score += game->board[k-1][j];  // Atualiza a pontuação
                        game->board[k][j] = 0;  // Limpa a célula onde a peça foi combinada
                        merged[k-1][j] = true;  // Marca a célula como já combinada
                        moved = true;  // Marca que houve movimento
                        // Se o valor atingiu o WIN_SCORE (pontuação para ganhar), marca a vitória
                        if (game->board[k-1][j] >= WIN_SCORE) {
                            game->winscore_reached = true;
                        }
                    }
                }
            }
        }
        break;

    case DOWN:  // Direção para baixo
        for (j = 0; j < tam; j++) {  // Percorre todas as colunas (de 0 a tam-1)
            for (i = tam-2; i >= 0; i--) {  // Começa pela penúltima linha (i=tam-2), pois a última linha não pode mover para baixo
                if (game->board[i][j] != 0) {  // Se encontrar um número diferente de 0, ou seja, uma peça
                    k = i;  // Armazena a linha atual em k (linha onde a peça foi encontrada)
                    // Move a peça para baixo, procurando por espaços vazios
                    while (k < tam-1 && game->board[k+1][j] == 0) {  // Enquanto houver uma célula vazia abaixo
                        game->board[k+1][j] = game->board[k][j];  // Move a peça para baixo
                        game->board[k][j] = 0;  // Limpa a célula onde a peça estava
                        k++;  // Move o índice k para baixo (linha abaixo)
                        moved = true;  // Marca que houve movimento
                    }
                    // Tenta combinar as peças (mesmo valor e não combinadas ainda)
                    if (k < tam-1 && game->board[k+1][j] == game->board[k][j] && !merged[k+1][j] && !merged[k][j]) {
                        game->board[k+1][j] *= 2;  // Combina as peças (multiplica por 2)
                        game->score += game->board[k+1][j];  // Atualiza a pontuação
                        game->board[k][j] = 0;  // Limpa a célula onde a peça foi combinada
                        merged[k+1][j] = true;  // Marca a célula como já combinada
                        moved = true;  // Marca que houve movimento
                        // Se o valor atingiu o WIN_SCORE (pontuação para ganhar), marca a vitória
                        if (game->board[k+1][j] >= WIN_SCORE) {
                            game->winscore_reached = true;
                        }
                    }
                }
            }
        }
        break;

    case LEFT:  // Direção para a esquerda
        for (i = 0; i < tam; i++) {  // Percorre todas as linhas (de 0 a tam-1)
            for (j = 1; j < tam; j++) {  // Começa pela segunda coluna (j=1), pois a primeira coluna não pode mover para a esquerda
                if (game->board[i][j] != 0) {  // Se encontrar um número diferente de 0, ou seja, uma peça
                    k = j;  // Armazena a coluna atual em k (coluna onde a peça foi encontrada)
                    // Move a peça para a esquerda, procurando por espaços vazios
                    while (k > 0 && game->board[i][k-1] == 0) {  // Enquanto houver uma célula vazia à esquerda
                        game->board[i][k-1] = game->board[i][k];  // Move a peça para a esquerda
                        game->board[i][k] = 0;  // Limpa a célula onde a peça estava
                        k--;  // Move o índice k para a esquerda (coluna anterior)
                        moved = true;  // Marca que houve movimento
                    }
                    // Tenta combinar as peças (mesmo valor e não combinadas ainda)
                    if (k > 0 && game->board[i][k-1] == game->board[i][k] && !merged[i][k-1] && !merged[i][k]) {
                        game->board[i][k-1] *= 2;  // Combina as peças (multiplica por 2)
                        game->score += game->board[i][k-1];  // Atualiza a pontuação
                        game->board[i][k] = 0;  // Limpa a célula onde a peça foi combinada
                        merged[i][k-1] = true;  // Marca a célula como já combinada
                        moved = true;  // Marca que houve movimento
                        // Se o valor atingiu o WIN_SCORE (pontuação para ganhar), marca a vitória
                        if (game->board[i][k-1] >= WIN_SCORE) {
                            game->winscore_reached = true;
                        }
                    }
                }
            }
        }
        break;

    case RIGHT:  // Direção para a direita
        for (i = 0; i < tam; i++) {  // Percorre todas as linhas (de 0 a tam-1)
            for (j = tam-2; j >= 0; j--) {  // Começa pela penúltima coluna (j=tam-2), pois a última coluna não pode mover para a direita
                if (game->board[i][j] != 0) {  // Se encontrar um número diferente de 0, ou seja, uma peça
                    k = j;  // Armazena a coluna atual em k (coluna onde a peça foi encontrada)
                    // Move a peça para a direita, procurando por espaços vazios
                    while (k < tam-1 && game->board[i][k+1] == 0) {  // Enquanto houver uma célula vazia à direita
                        game->board[i][k+1] = game->board[i][k];  // Move a peça para a direita
                        game->board[i][k] = 0;  // Limpa a célula onde a peça estava
                        k++;  // Move o índice k para a direita (coluna seguinte)
                        moved = true;  // Marca que houve movimento
                    }
                    // Tenta combinar as peças (mesmo valor e não combinadas ainda)
                    if (k < tam-1 && game->board[i][k+1] == game->board[i][k] && !merged[i][k+1] && !merged[i][k]) {
                        game->board[i][k+1] *= 2;  // Combina as peças (multiplica por 2)
                        game->score += game->board[i][k+1];  // Atualiza a pontuação
                        game->board[i][k] = 0;  // Limpa a célula onde a peça foi combinada
                        merged[i][k+1] = true;  // Marca a célula como já combinada
                        moved = true;  // Marca que houve movimento
                        // Se o valor atingiu o WIN_SCORE (pontuação para ganhar), marca a vitória
                        if (game->board[i][k+1] >= WIN_SCORE) {
                            game->winscore_reached = true;
                        }
                    }
                }
            }
        }
        break;
}


    if (moved) {
        add_random_number(game); // Só adiciona um número aleatório se houve movimento
        game->moves++; //inc num de movimentos apenas se realmente houve mudança no board
    }
    else{
        printw("Movimento invalido");
    }
}

void quit_game(Game *game) {
    print_board(game);  // Exibe o estado final do tabuleiro
    printw("\nJogo encerrado. Pressione qualquer tecla para sair...\n");

    // Verifica se o jogo ainda não acabou
    if (game->game_over == false) {
        printw("Deseja salvar o jogo antes de sair? (S/N): ");
        refresh();  // Atualiza a tela para garantir que a mensagem apareça

        int ch = getch(); // Obtém a tecla pressionada
        if (ch == 's' || ch == 'S') {
            save_game(game, "game_save.txt");  // Salva o jogo se o usuário pressionar 's' ou 'S'
            printw("Jogo salvo com sucesso!\n");
        } 
        else{
            printw("Jogo não salvo");
            refresh();
        }
    }

    refresh();  // Atualiza a tela para garantir que o texto apareça
    getch();    // Aguarda o usuário pressionar uma tecla

    free_game(game); // Libera a memória alocada para o jogo
    endwin();   // Fecha a janela do ncurses
    exit(0);    // Encerra o programa
}

int menu() {
    int option = 0;
    keypad(stdscr, TRUE); // Para reconhecer as teclas direcionais
    noecho(); // Para não exibir as teclas pressionadas na tela
    while (1) {
        printw("Escolha uma opção:\n");
        printw("1 - Iniciar Novo Jogo\n");
        printw("2 - Continuar Jogo\n");
        printw("3 - Sair\n");
        refresh();
        option = getch();

        if (option == '1') {
            return 1;
        } else if (option == '2') {
            return 2;
        } else if (option == '3') {
            return 3;
        }
    }
}

void show_help() {
    FILE *fp = fopen("help.txt", "r"); // Abre o arquivo de ajuda
    if (fp == NULL) {
        printw("Erro ao abrir o arquivo de ajuda.\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        printw("%s", line);  // Lê e imprime o conteúdo do arquivo
    }
    fclose(fp);  // Fecha o arquivo
    printw("\nPressione qualquer tecla para voltar ao jogo...\n");
    getch();
}

bool check_win(Game *game) {
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if (game->board[i][j] >= WIN_SCORE) {
                game->winscore_reached = true;
                return true;
            }
        }
    }
    return false;
}

bool can_move(const Game *game) {
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if (game->board[i][j] == 0) {
                return true; // Ainda há espaços vazios
            }
            // Verificar movimentos horizontais
            if (j < tam - 1 && game->board[i][j] == game->board[i][j + 1]) {
                return true; // Blocos prox iguais
            }
            // Verificar movimentos verticais
            if (i < tam - 1 && game->board[i][j] == game->board[i + 1][j]) {
                return true; // Blocos prox iguais
            }
        }
    }
    return false; // Nenhum movimento possível
}

void handle_input(Game *game) {
    // Se o jogo já acabou, ainda permite que o jogador pressione 'q' para sair
    if (game->game_over) {
        int ch = getch(); // Captura a tecla pressionada

        if (ch == 'q' || ch == 'Q') {
            quit_game(game);  // Sai do jogo
        }
        return; // Não processa mais entradas caso o jogo tenha acabado
    }

    // Caso contrário, processa as entradas normalmente enquanto o jogo não acabou
    int ch = getch(); // Obtém a tecla pressionada pelo jogador
    switch (ch) {
        case KEY_UP:    // Se pressionar a seta para cima
        case 'w':       // Ou a tecla 'w' para cima
            move_board(game, UP);  // Chama a função para mover o tabuleiro para cima
            break;
        case KEY_DOWN:  // Se pressionar a seta para baixo
        case 's':       // Ou a tecla 's' para baixo
            move_board(game, DOWN);  // Chama a função para mover o tabuleiro para baixo
            break;
        case KEY_LEFT:  // Se pressionar a seta para a esquerda
        case 'a':       // Ou a tecla 'a' para a esquerda
            move_board(game, LEFT);  // Chama a função para mover o tabuleiro para a esquerda
            break;
        case KEY_RIGHT: // Se pressionar a seta para a direita
        case 'd':       // Ou a tecla 'd' para a direita
            move_board(game, RIGHT); // Chama a função para mover o tabuleiro para a direita
            break;
        case 'z':       // Se pressionar a tecla 'z' (undo)
            undo_move(game);  // Chama a função para desfazer o último movimento
            break;
        case 'q':       // Se pressionar a tecla 'q' (sair)
            quit_game(game);  // Chama a função para sair do jogo
            break;
        case 'h':       // Se pressionar a tecla 'h' (ajuda)
            show_help();  // Chama a função para mostrar a ajuda
            break;
        case 'p':
            change_value(game);
            break;
    }
}

void loop_jogo(Game *game) {
    while (!game->game_over) {
        clear();  // Limpa a tela

        print_board(game);

        printw("\nScore: %d | Highscore: %d | Moves: %d\n", game->score, game->highscore, game->moves);
        
        refresh();  // Atualiza a tela após exibir as informações

        if (game->winscore_reached) { // Verifica se o jogador atingiu a pontuação de vitória
            printw("Parabéns! Você atingiu a pontuação de vitória (%d)!\n", WIN_SCORE);
            printw("Pressione J para continuar jogando ou Q para sair.\n");
            refresh();  // Atualiza a tela para mostrar a mensagem

            int ch = getch();
            if (ch == 'Q' || ch == 'q') {
                game->game_over = true;
            } else if (ch == 'J' || ch == 'j') {
                game->winscore_reached = false; // Permite continuar jogando
            }
        } else {
            handle_input(game);  // Processa a entrada do jogador

            if (check_full(game) && !can_move(game)) {
                clear(); // Limpa a tela antes de mostrar a mensagem de fim de jogo
                printw("Fim de jogo! Não há mais movimentos possíveis.\n");
                printw("Pressione qualquer tecla para sair.\n");
                refresh(); // Atualiza a tela para mostrar o fim de jogo
                getch();  // Aguarda a interação do jogador para encerrar
                game->game_over = true;
            }
            
        }
        update_score(game); // Atualiza o score e o highscore
    }
}

int main() {
    initscr();     // Inicializa a interface gráfica do ncurses
    cbreak(); // Desativa o buffer de entrada, para responder rapidamente
    keypad(stdscr, TRUE); // Habilita captura de setas
    noecho(); // Para não exibir as teclas pressionadas na tela
    srand(time(NULL));// Inicializa gerador de números aleatórios com o tempo atual
    curs_set(0); // Esconde o cursor
    init_colors(); // Inicializa as cores

    Game game;
    init_board(&game);
    load_highscore(&game.highscore);

    int option = menu();
    if (option == 1) {
        clear();
        printw("Novo Jogo!\n");
    } else if (option == 2) {
        load_game(&game, "game_save.txt");
    } 
    else {
        // quit_game(&game);
        endwin();
        return 0;
    }

    loop_jogo(&game);
    sleep(0.5);

    endwin();  // Finaliza a interface ncurses
    return 0;
}
