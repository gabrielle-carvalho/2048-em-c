#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "game.h"

// Inicializa o tabuleiro com valores iniciais e aloca memória dinamicamente
void init_board(Game *game) {
    // Alocação de memória para o tabuleiro e backups
    game->board = (int **)malloc(BOARD_SIZE * sizeof(int *));
    game->prev_board = (int **)malloc(BOARD_SIZE * sizeof(int *));
    game->backup_board = (int **)malloc(BOARD_SIZE * sizeof(int *));
    if (!game->board || !game->prev_board || !game->backup_board) {
        perror("Falha na alocação de memória.");
        exit(1); // Encerra o programa em caso de falha
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        game->board[i] = (int *)malloc(BOARD_SIZE * sizeof(int));
        game->prev_board[i] = (int *)malloc(BOARD_SIZE * sizeof(int));
        game->backup_board[i] = (int *)malloc(BOARD_SIZE * sizeof(int));
        if (!game->board[i] || !game->prev_board[i] || !game->backup_board[i]) {
            perror("Falha na alocação de memória para uma linha.");
            exit(1);
        }
        memset(game->board[i], 0, BOARD_SIZE * sizeof(int)); // Inicializa com 0
    }

    game->score = 0;
    game->moves = 0;
    game->game_over = false;
    game->backup_saved = false;
    game->winscore_reached = false;
    add_random_number(game);
    add_random_number(game);
}

// Libera a memória alocada para o jogo
void free_game(Game *game) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        free(game->board[i]);
        free(game->prev_board[i]);
        free(game->backup_board[i]);
    }
    free(game->board);
    free(game->prev_board);
    free(game->backup_board);
}

// Realiza o backup do estado atual do jogo
void backup(Game *game) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        memcpy(game->backup_board[i], game->board[i], BOARD_SIZE * sizeof(int));
    }
    game->backup_score = game->score;
    game->backup_moves = game->moves;
    game->backup_saved = true;
}

// Desfaz a última jogada (undo)
void undo_move(Game *game) {
    if (!game->backup_saved) {
        printf("Nenhuma jogada para desfazer!\n");
        return;
    }
    game->score = game->backup_score;
    game->moves = game->backup_moves;
    for (int i = 0; i < BOARD_SIZE; i++) {
        memcpy(game->board[i], game->backup_board[i], BOARD_SIZE * sizeof(int));
    }
    game->backup_saved = false;
    printf("Última jogada desfeita.\n");
}

// Verifica se o tabuleiro está cheio
bool check_full(const Game *game) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (game->board[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

// Adiciona um número aleatório (2 ou 4) em uma posição vazia do tabuleiro
void add_random_number(Game *game) {
    if (check_full(game)) {
        return;
    }
    int i, j;
    do {
        i = rand() % BOARD_SIZE;
        j = rand() % BOARD_SIZE;
    } while (game->board[i][j] != 0);

    game->board[i][j] = (rand() % 10 < 6) ? 2 : 4;
}

// Imprime o tabuleiro no console
void print_board(const Game *game) {
    system("clear || cls");
    printf("-----------------------------\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("|");
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (game->board[i][j] == 0) {
                printf("      |");
            } else {
                printf(" %4d |", game->board[i][j]);
            }
        }
        printf("\n");
        printf("-----------------------------\n");
    }
    printf("Score: %d | Highscore: %d | Moves: %d\n", game->score, game->highscore, game->moves);
}

// Salva o estado do jogo em um arquivo
void save_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Erro ao salvar o jogo");
        return;
    }

    fwrite(&game->score, sizeof(int), 1, file);
    fwrite(&game->highscore, sizeof(int), 1, file);
    fwrite(&game->moves, sizeof(int), 1, file);

    for (int i = 0; i < BOARD_SIZE; i++) {
        fwrite(game->board[i], sizeof(int), BOARD_SIZE, file);
        fwrite(game->prev_board[i], sizeof(int), BOARD_SIZE, file);
        fwrite(game->backup_board[i], sizeof(int), BOARD_SIZE, file);
    }

    fclose(file);
    printf("Jogo salvo em: %s\n", filename);
}

// Carrega o estado do jogo de um arquivo
void load_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "rb"); //read binario
    if (file == NULL) {
        perror("Erro ao carregar o jogo");
        return;
    }
    // Aloca memória para as estruturas 
    game->board = (int **) malloc(tam * sizeof(int *));
    game->prev_board = (int **) malloc(tam * sizeof(int *));
    game->backup_board = (int **) malloc(tam * sizeof(int *));
    if (game->board == NULL || game->prev_board == NULL || game->backup_board == NULL) {
        perror("Erro ao alocar memória para o jogo");
        fclose(file);
        return;
    }
    for (int i = 0; i < tam; i++) {// Aloca memória para cada linha
        game->board[i] = (int *) malloc(tam * sizeof(int));
        game->prev_board[i] = (int *) malloc(tam * sizeof(int));
        game->backup_board[i] = (int *) malloc(tam * sizeof(int)); 
        if (game->board[i] == NULL || game->prev_board[i] == NULL || game->backup_board[i] == NULL) {
            perror("Erro ao alocar memória para uma linha");
            fclose(file);
            return;
        }
    }
    // Lê os dados do arquivo para o estado do jogo
    fread(&game->score, sizeof(int), 1, file); //carrega o score no momento que foi sslvo
    fread(&game->highscore, sizeof(int), 1, file); // carrega o highscore
    fread(&game->moves, sizeof(int), 1, file);  // Carregar o número de movimentos

    for (int i = 0; i < tam; i++) {    // Lê os boards do arquivo
        fread(game->board[i], sizeof(int), tam, file);
        fread(game->prev_board[i], sizeof(int), tam, file);
        fread(game->backup_board[i], sizeof(int), tam, file);
    }

    fclose(file);
    printf("Jogo carregado com sucesso!\n");
}

void update_score(int new_value, Game *game) {
    if (game->score > game->highscore) {
        game->highscore = game->score;
        save_highscore(game);  // Salva o novo highscore no arquivo
    }
}

void save_highscore(const Game *game) { //salva o highscore num arquivo txt para ser disponibilizado durante o jogo
    FILE *file = fopen("highscore.txt", "w");
    if (!file) {
        perror("Erro ao salvar highscore");
        return;
    }
    fprintf(file, "%d\n", game->highscore);
    fclose(file);
}

void load_highscore(int *highscore) { //atribui o conteudo de file a variavel highscore
    FILE *file = fopen("highscore.txt", "r");
    if (!file) {
        *highscore = 0;
        return;
    }
    fscanf(file, "%d", highscore);
    fclose(file);
}
void move_board(Game *game, Direction dir) {
    int i, j, k;
    bool moved = false;
    backup(game);

    switch (dir) {
        case UP:
            for (j = 0; j < tam; j++) { //percorre a coluna
                int move = 0;
                bool merged[tam] = {false};  // Initialize all blocks as not merged
                for (i = 1; i < tam; i++) { //percorre as linhas
                    if (game->board[i][j] != 0) { 
                        k = i;
                        while (k > move && game->board[k - 1][j] == 0) {
                            game->board[k - 1][j] = game->board[k][j];
                            game->board[k][j] = 0;
                            k--;
                            moved = true;
                        }
                        if (k > move && game->board[k - 1][j] == game->board[k][j] && !merged[k - 1] && !merged[k]) {
                            game->board[k - 1][j] *= 2;
                            game->score += game->board[k - 1][j];
                            game->board[k][j] = 0;
                            merged[k - 1] = true;
                            moved = true;
                            move = k - 1;
                        } else {
                            move = k;
                        }
                    }
                }
            }
            break;

        case DOWN:
            for (j = 0; j < tam; j++) {
                int move = tam - 1;
                bool merged[tam] = {false};  // Initialize all blocks as not merged
                for (i = tam - 2; i >= 0; i--) {
                    if (game->board[i][j] != 0) {
                        k = i;
                        while (k < tam - 1 && game->board[k + 1][j] == 0) {
                            game->board[k + 1][j] = game->board[k][j];
                            game->board[k][j] = 0;
                            k++;
                            moved = true;
                        }
                        if (k < tam - 1 && game->board[k + 1][j] == game->board[k][j] && !merged[k + 1] && !merged[k]) {
                            game->board[k + 1][j] *= 2;
                            game->score += game->board[k + 1][j];
                            game->board[k][j] = 0;
                            merged[k + 1] = true;
                            moved = true;
                            move = k + 1;
                        } else {
                            move = k;
                        }
                    }
                }
            }
            break;

        case LEFT:
            for (i = 0; i < tam; i++) {
                int move = 0;
                bool merged[tam] = {false};  // Initialize all blocks as not merged
                for (j = 1; j < tam; j++) {
                    if (game->board[i][j] != 0) {
                        k = j;
                        while (k > move && game->board[i][k - 1] == 0) {
                            game->board[i][k - 1] = game->board[i][k];
                            game->board[i][k] = 0;
                            k--;
                            moved = true;
                        }
                        if (k > move && game->board[i][k - 1] == game->board[i][k] && !merged[k - 1] && !merged[k]) {
                            game->board[i][k - 1] *= 2;
                            game->score += game->board[i][k - 1];
                            game->board[i][k] = 0;
                            merged[k - 1] = true;
                            moved = true;
                            move = k - 1;
                        } else {
                            move = k;
                        }
                    }
                }
            }
            break;

        case RIGHT:
            for (i = 0; i < tam; i++) {
                int move = tam - 1;
                bool merged[tam] = {false};  // Initialize all blocks as not merged
                for (j = tam - 2; j >= 0; j--) {
                    if (game->board[i][j] != 0) {
                        k = j;
                        while (k < tam - 1 && game->board[i][k + 1] == 0) {
                            game->board[i][k + 1] = game->board[i][k];
                            game->board[i][k] = 0;
                            k++;
                            moved = true;
                        }
                        if (k < tam - 1 && game->board[i][k + 1] == game->board[i][k] && !merged[k + 1] && !merged[k]) {
                            game->board[i][k + 1] *= 2;
                            game->score += game->board[i][k + 1];
                            game->board[i][k] = 0;
                            merged[k + 1] = true;
                            moved = true;
                            move = k + 1;
                        } else {
                            move = k;
                        }
                    }
                }
            }
            break;
    }

    if (moved) {
        add_random_number(game);
        add_random_number(game);
        game->moves++;
    } else {
        printf("Movimento inválido!\n");
    }
}
