#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h> //setas teclado 

#define WIN_SCORE 2048
#define tam 4

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

bool check_win(const Game *game) { //percorre o array e se o winscore estiver em alguma posicao ele retorna true
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if (game->board[i][j] == WIN_SCORE) {
                return 1;
            }
        }
    }
    return 0;
}

int can_move(const Game *game) { 
    if (!check_full(game)) {
        return 1; //se nao estiver cheio continua o jogo
    }
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) { //se tiver cheio mas existem cominaçoes possiveis, continua
            if ((i + 1 < tam && game->board[i][j] == game->board[i+1][j]) ||
                (j + 1 < tam && game->board[i][j] == game->board[i][j+1])) {
                return 1;
            }
        }
    }
    printf("Não há mais jogadas possíveis.");
    menu(game);
    return 0; // não há mais jogadas possíveis
}