#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h> //setas teclado 

#define WIN_SCORE 2048
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

void free_game(Game *game) { //quando chamado libera a memoria das alocações feitas dinamicamente
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
    for (int i = 0; i < tam; i++) { //copia o que esta no board no momento para o backup, caso queira fazer o undo
        memcpy(game->backup_board[i], game->board[i], tam * sizeof(int));
    }
    game->backup_score = game->score; //salva o score do momento para recuperar caso undo
    game->backup_moves = game->moves; // salva a quantidade de movimentos para cajo a jogada seja desfeita
    game->backup_saved = true; //diz que o jogo ja foi salvo
}

void undo_move(Game *game) {
    if (!game->backup_saved) {
        printf("Nenhuma jogada para desfazer!\n"); //retorna isso caso o usuario tente fazer mais que um undo
        return;
    }
    game->score = game->backup_score; //restaura o score
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
    //MUDAR JEITO DE CRIAR NUMEROS ALEATORIOS?
}

void print_board(const Game *game) { //print o jogo em si na tela
    system("clear || cls"); //nao deixa os boards acumularem
    printf("-----------------------------\n");
    for (int i = 0; i < tam; i++) {
        printf("|");
        for (int j = 0; j < tam; j++) {
            if(game->board[i][j] == 0){ // se o valor for 0 ele deixa vazio
                printf("      |");
            }
            else{
                printf(" %4d |", game->board[i][j]);
            }
        }
        printf("\n");
        printf("-----------------------------\n");    
    }
    printf("Score: %d | Highscore: %d | Moves: %d\n", game->score, game->highscore, game->moves);
}

void save_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "wb"); //write binary
    if(file == NULL){
        perror("Erro ao salvar o jogo");
        return;
    }

    fwrite(&game->score, sizeof(int), 1, file);
    fwrite(&game->highscore, sizeof(int), 1, file);
    fwrite(&game->moves, sizeof(int), 1, file);  

    for(int i=0; i<tam; i++){
        fwrite(game->board[i], sizeof(int), tam, file);
        fwrite(game->prev_board[i], sizeof(int), tam, file);
        fwrite(game->backup_board[i], sizeof(int), tam, file);
    }
    fclose(file);
    printf("Jogo salvo em: %s", filename);
}

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