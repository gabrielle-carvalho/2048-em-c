#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h> //setas teclado 

#define WIN_SCORE 32
#define tam 4

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    int **board;
    int **prev_board; //retaura estado do tabuleiro no undo
    int score;
    int highscore;
    int moves;
    bool game_over;
    bool backup_saved;  // Flag para verificar se o jogo foi salvo
    int backup_score; //restaura o score no undo
    int backup_moves; // restaura qtd de moves no undo
    int **backup_board; //recupera tabuleiro salvo
    bool winscore_reached; // flag para verificar se winscore foi atingido
} Game;

void init_board(Game *game), quit_game(Game *game), free_game(Game *game), print_board(const Game *game), backup(Game *game), update_score(Game *game), save_highscore(const Game *game), add_random_number(Game *game), move_board(Game *game, Direction dir), save_game(Game *game, const char *filename), load_game(Game *game, const char *filename), load_highscore(int *highscore), print_fixed_menu();
int can_move(const Game *game), menu();
bool check_win(Game *game), check_full(const Game *game);

void init_board(Game *game) {
    game->board = (int **) calloc(tam, sizeof(int *));
    game->prev_board = (int **) calloc(tam, sizeof(int *));
    game->backup_board = (int **) malloc(tam * sizeof(int *));
    if (!game->board || !game->prev_board || !game->backup_board) {
        perror("Falha na alocação de memória.");
        return;
    }

    for (int i = 0; i < tam; i++) {
        game->board[i] = (int *) calloc(tam, sizeof(int));
        game->prev_board[i] = (int *) calloc(tam, sizeof(int));
        game->backup_board[i] = (int *) malloc(tam * sizeof(int));
        if (!game->board[i] || !game->prev_board[i] || !game->backup_board[i]) {
            perror("Falha na alocação de memória para uma linha.");
            return;
        }
    }
    game->score = 0;
    game->moves = 0;
    game->game_over = false;
    game->backup_saved = false;

    add_random_number(game);
    add_random_number(game);
}

void free_game(Game *game) {
    for (int i = 0; i < tam; i++) {
        free(game->board[i]);
        free(game->prev_board[i]);
        free(game->backup_board[i]);
    }
    free(game->board);
    free(game->prev_board);
    free(game->backup_board);
}

void backup(Game *game) {
    for (int i = 0; i < tam; i++) {
        memcpy(game->backup_board[i], game->board[i], tam * sizeof(int));
    }
    game->backup_score = game->score;
    game->backup_moves = game->moves;
    game->backup_saved = true;
}

void undo_move(Game *game) {
    if (!game->backup_saved) {
        printf("Nenhuma jogada para desfazer!\n");
        return;
    }
    game->score = game->backup_score;
    game->moves = game->backup_moves; // Restaurar o número de movimentos

    for (int i = 0; i < tam; i++) {
        memcpy(game->board[i], game->backup_board[i], tam * sizeof(int));// Restaura o estado do jogo a partir do backup
    }
    game->backup_saved = false;  // Resetar o estado de backup
    printf("Última jogada desfeita.\n");
}

bool check_full(const Game *game) {
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if (game->board[i][j] == 0) { //percorre todo array e ao encontrar espaco livre retorna 0
                return false;
            }
        }
    }
    return true;
}

void add_random_number(Game *game) {
    if (check_full(game)) {
        return; // Não há espaço para adicionar um novo número
    }
    int i, j;
    do {
        i = rand() % tam;
        j = rand() % tam;
    } while (game->board[i][j] != 0);

    game->board[i][j] = (rand() % 10 < 6) ? 2 : 4;
}

void print_board(const Game *game) {
    clear(); // limpa a tela antes de exibir o novo tabuleiro
    print_fixed_menu();  // Exibe o menu fixo sempre no topo
    printw("-----------------------------\n");
    for (int i = 0; i < tam; i++) {
        printw("|");
        for (int j = 0; j < tam; j++) {
            if (game->board[i][j] == 0) {
                printw("      |");
            } else {
                printw(" %4d |", game->board[i][j]);
            }
        }
        printw("\n");
        printw("-----------------------------\n");
    }
    printw("Score: %d | Highscore: %d | Moves: %d\n", game->score, game->highscore, game->moves);
    refresh(); // atualiza a tela
}

void print_fixed_menu() {
    printw("Menu: [W] Move Up | [S] Move Down | [A] Move Left | [D] Move Right\n");
    printw("[Z] Undo | [B] Save | [Q] Quit | [H] Help\n");
}

void save_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Erro ao salvar o jogo");
        return;
    }

    // Salva o estado do jogo (pontuação, movimentos, tabuleiros)
    fwrite(&game->score, sizeof(int), 1, file);
    fwrite(&game->highscore, sizeof(int), 1, file);
    fwrite(&game->moves, sizeof(int), 1, file);  

    for (int i = 0; i < tam; i++) {
        fwrite(game->board[i], sizeof(int), tam, file);
        fwrite(game->prev_board[i], sizeof(int), tam, file);
        fwrite(game->backup_board[i], sizeof(int), tam, file);
    }

    fclose(file);
    printf("Jogo salvo em: %s\n", filename);
}

void load_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", filename);
        return;
    }

    if (game->board) { // Liberar memória dos tabuleiros se já foi alocada
        for (int i = 0; i < tam; i++) {
            free(game->board[i]);
            free(game->prev_board[i]);
            free(game->backup_board[i]);
        }
        free(game->board);
        free(game->prev_board);
        free(game->backup_board);
    }

    // Alocar memória corretamente para os tabuleiros
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

    // Ler o estado do jogo (tabuleiros, pontuação, etc.)
    fread(&game->score, sizeof(int), 1, file);
    fread(&game->highscore, sizeof(int), 1, file);
    fread(&game->moves, sizeof(int), 1, file);

    // Ler os tabuleiros
    for (int i = 0; i < tam; i++) {
        if (fread(game->board[i], sizeof(int), tam, file) != tam) {
            printf("Erro ao ler o tabuleiro (board) da linha %d.\n", i);
            fclose(file);
            return;
        }
        if (fread(game->prev_board[i], sizeof(int), tam, file) != tam) {
            printf("Erro ao ler o tabuleiro (prev_board) da linha %d.\n", i);
            fclose(file);
            return;
        }
        if (fread(game->backup_board[i], sizeof(int), tam, file) != tam) {
            printf("Erro ao ler o tabuleiro (backup_board) da linha %d.\n", i);
            fclose(file);
            return;
        }
    }

    fclose(file);
    printf("Jogo carregado com sucesso!\n");
}

void update_score(Game *game) {
    if (game->score > game->highscore) {
        game->highscore = game->score;
        save_highscore(game);  // Salva o novo highscore no arquivo
    }
}

void save_highscore(const Game *game) {
    FILE *file = fopen("highscore.dat", "wb");
    if (file) {
        fwrite(&game->highscore, sizeof(int), 1, file);
        fclose(file);
    }
}

void load_highscore(int *highscore) {
    FILE *file = fopen("highscore.dat", "rb");
    if (file) {
        fread(highscore, sizeof(int), 1, file);
        fclose(file);
    } else {
        *highscore = 0;
    }
}

void move_board(Game *game, Direction dir) {
    int i, j, k;
    bool merged[tam][tam] = {{false}};  // Controle de combinações
    bool moved = false;  // Flag para saber se houve movimento

    // Realiza o backup do estado do jogo, caso precise desfazer
    backup(game);

    switch (dir) {
        case UP:
            for (j = 0; j < tam; j++) {
                int last_merge = -1;  // A última linha onde houve uma combinação
                for (i = 1; i < tam; i++) {
                    if (game->board[i][j] != 0) {  // Encontrou um número diferente de 0
                        k = i;
                        // Move a peça para cima, procurando por espaços vazios
                        while (k > 0 && game->board[k-1][j] == 0) {
                            game->board[k-1][j] = game->board[k][j];
                            game->board[k][j] = 0;
                            k--;
                            moved = true;
                        }
                        // Tenta combinar as peças
                        if (k > 0 && game->board[k-1][j] == game->board[k][j] && !merged[k-1][j] && !merged[k][j]) {
                            game->board[k-1][j] *= 2;  // Combina as peças
                            game->score += game->board[k-1][j];  // Atualiza a pontuação
                            game->board[k][j] = 0;
                            merged[k-1][j] = true;  // Marca como já combinada
                            moved = true;
                        }
                    }
                }
            }
            break;

        case DOWN:
            for (j = 0; j < tam; j++) {
                int last_merge = tam;  // A última linha onde houve uma combinação
                for (i = tam-2; i >= 0; i--) {
                    if (game->board[i][j] != 0) {  // Encontrou um número diferente de 0
                        k = i;
                        // Move a peça para baixo, procurando por espaços vazios
                        while (k < tam-1 && game->board[k+1][j] == 0) {
                            game->board[k+1][j] = game->board[k][j];
                            game->board[k][j] = 0;
                            k++;
                            moved = true;
                        }
                        // Tenta combinar as peças
                        if (k < tam-1 && game->board[k+1][j] == game->board[k][j] && !merged[k+1][j] && !merged[k][j]) {
                            game->board[k+1][j] *= 2;  // Combina as peças
                            game->score += game->board[k+1][j];  // Atualiza a pontuação
                            game->board[k][j] = 0;
                            merged[k+1][j] = true;  // Marca como já combinada
                            moved = true;
                        }
                    }
                }
            }
            break;

        case LEFT:
            for (i = 0; i < tam; i++) {
                int last_merge = -1;  // A última coluna onde houve uma combinação
                for (j = 1; j < tam; j++) {
                    if (game->board[i][j] != 0) {  // Encontrou um número diferente de 0
                        k = j;
                        // Move a peça para a esquerda
                        while (k > 0 && game->board[i][k-1] == 0) {
                            game->board[i][k-1] = game->board[i][k];
                            game->board[i][k] = 0;
                            k--;
                            moved = true;
                        }
                        // Tenta combinar as peças
                        if (k > 0 && game->board[i][k-1] == game->board[i][k] && !merged[i][k-1] && !merged[i][k]) {
                            game->board[i][k-1] *= 2;
                            game->score += game->board[i][k-1];
                            game->board[i][k] = 0;
                            merged[i][k-1] = true;
                            moved = true;
                        }
                    }
                }
            }
            break;

        case RIGHT:
            for (i = 0; i < tam; i++) {
                int last_merge = tam;  // A última coluna onde houve uma combinação
                for (j = tam-2; j >= 0; j--) {
                    if (game->board[i][j] != 0) {  // Encontrou um número diferente de 0
                        k = j;
                        // Move a peça para a direita
                        while (k < tam-1 && game->board[i][k+1] == 0) {
                            game->board[i][k+1] = game->board[i][k];
                            game->board[i][k] = 0;
                            k++;
                            moved = true;
                        }
                        // Tenta combinar as peças
                        if (k < tam-1 && game->board[i][k+1] == game->board[i][k] && !merged[i][k+1] && !merged[i][k]) {
                            game->board[i][k+1] *= 2;
                            game->score += game->board[i][k+1];
                            game->board[i][k] = 0;
                            merged[i][k+1] = true;
                            moved = true;
                        }
                    }
                }
            }
            break;
    }

    if (moved) {
        add_random_number(game);  // Adiciona um número aleatório após o movimento
        game->moves++;  // Incrementa o número de movimentos
    }
}

void quit_game(Game *game) {
    free_game(game); // Libera a memória alocada para o jogo
    endwin(); // Fecha a janela do terminal
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
    if (game->score >= WIN_SCORE) {
        game->winscore_reached = true;  // Marca que a pontuação vencedora foi atingida
        return true;
    }
    return false;
}

int can_move(const Game *game) { 
    if (!check_full(game)) {
        return 1; //se nao estiver cheio continua o jogo
    }
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) { //se tiver cheio mas existem cominaçoes possiveis, continua
            if (j + 1 < tam && game->board[i][j] == game->board[i][j + 1]) {
                return 1;  // Movimento possível à direita
            }

            // Verificar células abaixo
            if (i + 1 < tam && game->board[i][j] == game->board[i + 1][j]) {
                return 1;  // Movimento possível para baixo
            }
        }
    }
    printf("Não há mais jogadas possíveis.");
    return 0; // não há mais jogadas possíveis
}

void handle_input(Game *game) {
    int ch = getch(); // Obtém a tecla pressionada
    switch (ch) {
        case KEY_UP:    // Setas
        case 'w':       // Move para cima
            move_board(game, UP);
            break;
        case KEY_DOWN:
        case 's':       // Move para baixo
            move_board(game, DOWN);
            break;
        case KEY_LEFT:
        case 'a':       // Move para esquerda
            move_board(game, LEFT);
            break;
        case KEY_RIGHT:
        case 'd':       // Move para direita
            move_board(game, RIGHT);
            break;
        case 'z':       // Undo
            undo_move(game);
            break;
        case 'b':       // Salvar
            save_game(game, "game_save.dat");
            break;
        case 'q':       // Sair
            quit_game(game);
            break;
        case 'h':
            show_help();
            break;
        case 'k':
            game->game_over = true;  // Finaliza o jogo
            break;
        case 'j':
            game->game_over = false;
            break;
            
    }
}

void loop_jogo(Game *game) {
    while (1) {
        print_board(game);
        handle_input(game);

        if (can_move(game) == 0) {
            printw("Fim de jogo!\n");
            printw("Pontuação Final: %d\n", game->score);
            printw("Pontuação Máxima: %d\n", game->highscore);
            refresh();
            game->game_over = true;
            break;
        }

        if (check_win(game) && !game->winscore_reached) {
            printw("Você venceu! Pressione qualquer tecla para continuar.\n");
            getch();
            break;
        }
    }
}

int main() {
    initscr(); // Inicializa ncurses
    cbreak(); // Desativa o buffer de entrada, para responder rapidamente
    keypad(stdscr, TRUE); // Habilita captura de setas
    noecho(); // Para não exibir as teclas pressionadas na tela
    srand(time(NULL));

    Game game;
    init_board(&game);
    load_highscore(&game.highscore);

    int option = menu();
    if (option == 1) {
        clear();
        printw("Novo Jogo!\n");
    } else if (option == 2) {
        load_game(&game, "game.dat");
    } else {
        quit_game(&game);
        endwin();
        return 0;
    }

    // Continue o loop do jogo aqui
    // Dentro do loop principal onde as teclas são verificadas
    while (!game.game_over) {
        print_board(&game);
        int ch = getch();

        // Verifica se a tecla pressionada é uma das teclas de movimento
        if (ch == 'q' || ch == 'Q') {
            break;
        }
        if (ch == KEY_UP || ch == 'w' || ch == 'W') {
            move_board(&game, UP);  // Move para cima se for a seta para cima ou W
        }
        else if (ch == KEY_DOWN || ch == 's' || ch == 'S') {
            move_board(&game, DOWN);  // Move para baixo se for a seta para baixo ou S
        }
        else if (ch == KEY_LEFT || ch == 'a' || ch == 'A') {
            move_board(&game, LEFT);  // Move para a esquerda se for a seta para esquerda ou A
        }
        else if (ch == KEY_RIGHT || ch == 'd' || ch == 'D') {
            move_board(&game, RIGHT);  // Move para a direita se for a seta para direita ou D
        }
        else if (ch == 'z' || ch == 'Z') {
            undo_move(&game);  // Desfaz a última jogada
        }
        else if (ch == 'b' || ch == 'B') {
            save_game(&game, "game.dat");  // Salva o jogo
        }
    }
    quit_game(&game);
    endwin();  // Finaliza a interface ncurses
    return 0;
}

