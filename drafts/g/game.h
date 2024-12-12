#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define WIN_SCORE 2048
#define BOARD_SIZE 4

// Enumeração para representar as direções de movimento
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

// Estrutura para armazenar o estado do jogo
typedef struct {
    int **board;         // Tabuleiro atual
    int **prev_board;    // Tabuleiro anterior (para possível uso futuro)
    int **backup_board;  // Backup do tabuleiro para desfazer jogadas (undo)
    int score;          // Pontuação atual
    int highscore;      // Melhor pontuação
    int moves;          // Número de movimentos
    bool game_over;     // Indica se o jogo acabou
    bool backup_saved;   // Indica se um backup foi salvo
    int backup_score;   // Score do backup
    int backup_moves;   // Quantidade de moves do backup
    bool winscore_reached; // Indica se o score de vitoria foi atingido

} Game;

// Funções de inicialização e liberação de memória
void init_board(Game *game);
void free_game(Game *game);

// Funções de manipulação do tabuleiro
void add_random_number(Game *game);
void move_board(Game *game, Direction dir);
void backup(Game *game);
void undo_move(Game *game);

// Funções de verificação de estado do jogo
bool check_win(const Game *game);
bool check_full(const Game *game);
int can_move(const Game *game);

// Funções de pontuação e gerenciamento de arquivos
void update_score(int new_value, Game *game);
void save_highscore(const Game *game);
void load_highscore(int *highscore);
void save_game(Game *game, const char *filename);
void load_game(Game *game, const char *filename);

// Funções de interface com o usuário
void print_board(const Game *game);
int menu(Game *game);
void clear_input_buffer();

#endif