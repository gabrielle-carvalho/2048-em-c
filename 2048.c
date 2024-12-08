#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h> //setas teclado

//COMENTAR MELHOR O CODIGO
//REMOVER REDUNDANCIAS

//SALVAR JOGO AUTOMATICAMENTE?
// fazer menu fixo na tela, com a opção de salvar, recomecar e abrir
//pedir confirmaçao ao sair e ao salvar
//nao printar um novo jogo a cada jogada
//interface melhor?
//colocar mais interaçoes do usuario, principalmente quando algo da errado
//delay entre as jogadas
//capturar as teclas de mover além do a,w,s,d
//opção pra abrir um jogo novo de forma mais clara, colocar menu, falar que so pode abri um novo apos fechar o aberto e pedir pra confirmar
// Opção de continuar depois de 2048?
// set_difficulty(): Alterar a dificuldade (ex.: tempo de resposta ou número inicial de peças).
// Adicionar sistema de combos: Pontuação extra por combinações consecutivas.

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
    int **prev_board;
    int score;
    int highscore;
    int moves;
    bool game_over;
    bool backup_saved;  // Flag para verificar se o jogo foi salvo
    int backup_score;
    int backup_moves;
    int **backup_board;
} Game;

void init_board(Game *game), free_game(Game *game), print_board(const Game *game), backup(Game *game), restore_board(Game *game), update_score(int new_value, Game *game), save_highscore(const Game *game);
void add_random_number(Game *game), move_board(Game *game, Direction dir), save_game(Game *game, const char *filename), load_game(Game *game, const char *filename), load_highscore(int *highscore), clear_input_buffer(); 
int can_move(const Game *game), menu();
bool check_win(const Game *game), check_full(const Game *game);

void init_board(Game *game) {
    game->board = (int **) malloc(tam * sizeof(int *));
    game->prev_board = (int **) malloc(tam * sizeof(int *));

    for (int i = 0; i < tam; i++) {
        game->board[i] = (int *) malloc(tam * sizeof(int));
        game->prev_board[i] = (int *) malloc(tam * sizeof(int));
    }
    game->score = 0;
    game->moves = 0;
    game->game_over = false;
    game->backup_saved = false;

    game->backup_board = (int **) malloc(tam * sizeof(int *));
    for(int i=0; i<tam; i++){
        game->backup_board[i] = (int *) malloc(tam * sizeof(int));
    }
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

    // Restaura o estado do jogo a partir do backup
    game->score = game->backup_score;
    game->moves = game->backup_moves; // Restaurar o número de movimentos

    for (int i = 0; i < tam; i++) {
        memcpy(game->board[i], game->backup_board[i], tam * sizeof(int));
    }
    game->backup_saved = false;  // Resetar o estado de backup
    printf("Última jogada desfeita.\n");
}

void restore_board(Game *game) {
    for (int i = 0; i < tam; i++) {
        memcpy(game->board[i], game->prev_board[i], tam * sizeof(int));
    }
}

bool check_full(const Game *game) {
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if (game->board[i][j] == 0) {
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
    printf("-----------------------------\n");
    for (int i = 0; i < tam; i++) {
        printf("|");
        for (int j = 0; j < tam; j++) {
            printf(" %4d |", game->board[i][j]);
        }
        printf("\n");
        printf("-----------------------------\n");    
    }
    
    printf("Score: %d | Highscore: %d | Moves: %d\n", game->score, game->highscore, game->moves);
}

void save_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if(file == NULL){
        perror("Erro ao salvar o jogo");
        return;
    }
    fwrite(&game->score, sizeof(int), 1, file);
    fwrite(&game->highscore, sizeof(int), 1, file);
    fwrite(&game->moves, sizeof(int), 1, file);  // Salvar o número de movimentos


    for(int i=0; i<tam; i++){
        fwrite(game->board[i], sizeof(int), tam, file);
        fwrite(game->prev_board[i], sizeof(int), tam, file);
        fwrite(game->backup_board[i], sizeof(int), tam, file);
    }
    fclose(file);
    printf("Jogo salvo em: %s", filename);
}

void load_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erro ao carregar o jogo");
        return;
    }

    // Aloca memória para as estruturas board, prev_board e backup_board
    game->board = (int **) malloc(tam * sizeof(int *));
    game->prev_board = (int **) malloc(tam * sizeof(int *));
    game->backup_board = (int **) malloc(tam * sizeof(int *));  // Alocar para backup_board

    if (game->board == NULL || game->prev_board == NULL || game->backup_board == NULL) {
        perror("Erro ao alocar memória para o jogo");
        fclose(file);
        return;
    }

    // Aloca memória para cada linha de board, prev_board e backup_board
    for (int i = 0; i < tam; i++) {
        game->board[i] = (int *) malloc(tam * sizeof(int));
        game->prev_board[i] = (int *) malloc(tam * sizeof(int));
        game->backup_board[i] = (int *) malloc(tam * sizeof(int));  // Alocar para cada linha de backup_board
        if (game->board[i] == NULL || game->prev_board[i] == NULL || game->backup_board[i] == NULL) {
            perror("Erro ao alocar memória para uma linha de board, prev_board ou backup_board");
            fclose(file);
            return;
        }
    }

    // Lê os dados do arquivo para o estado do jogo
    fread(&game->score, sizeof(int), 1, file);
    fread(&game->highscore, sizeof(int), 1, file);
    fread(&game->moves, sizeof(int), 1, file);  // Carregar o número de movimentos

    // Lê os boards do arquivo
    for (int i = 0; i < tam; i++) {
        fread(game->board[i], sizeof(int), tam, file);
        fread(game->prev_board[i], sizeof(int), tam, file);
        fread(game->backup_board[i], sizeof(int), tam, file);  // Ler backup_board corretamente
    }

    fclose(file);
    printf("Jogo carregado com sucesso!\n");

    // Garantir que o backup_saved seja verdadeiro após carregar o jogo
    game->backup_saved = true;
}

void update_score(int new_value, Game *game) {
    if (game->score > game->highscore) {
        game->highscore = game->score;
        save_highscore(game);  // Salva o novo highscore no arquivo
    }
}

void save_highscore(const Game *game) {
    FILE *file = fopen("highscore.txt", "w");
    if (!file) {
        perror("Erro ao salvar highscore");
        return;
    }
    fprintf(file, "%d\n", game->highscore);
    fclose(file);
}

void load_highscore(int *highscore) {
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
    int old_score = game->score;
    backup(game);

    switch (dir) {
        case UP:
            for (j = 0; j < tam; j++) {
                for (i = 1; i < tam; i++) {
                    if (game->board[i][j] != 0) {
                        k = i;
                        while (k > 0 && game->board[k-1][j] == 0) {
                            game->board[k-1][j] = game->board[k][j];
                            game->board[k][j] = 0;
                            k--;
                            moved = 1;
                        }
                        if (k > 0 && game->board[k-1][j] == game->board[k][j]) {
                            game->board[k-1][j] *= 2; // Soma os blocos
                            game->score += game->board[k-1][j];  // Adiciona o valor da combinação à pontuação
                            game->board[k][j] = 0; // Zera o bloco combinado
                            moved = true;
                            k--;
                        } 

                    }
                }
            }
            break;
        case DOWN:
            for (j = 0; j < tam; j++) {
                for (i = tam - 2; i >= 0; i--) {
                    if (game->board[i][j] != 0) {
                        k = i;
                        while (k < tam - 1 && game->board[k+1][j] == 0) {
                            game->board[k+1][j] = game->board[k][j];
                            game->board[k][j] = 0;
                            k++;
                            moved = 1;
                        }
                        if (k < tam - 1 && game->board[k+1][j] == game->board[k][j]) {
                            game->board[k+1][j] *= 2;
                            game->score += game->board[k+1][j];
                            game->board[k][j] = 0;
                            moved = true;
                            k++;
                        }
                    }
                }
            }
            break;
        case LEFT:
            for (i = 0; i < tam; i++) {
                for (j = 1; j < tam; j++) {
                    if (game->board[i][j] != 0) {
                        k = j;
                        while (k > 0 && game->board[i][k-1] == 0) {
                            game->board[i][k-1] = game->board[i][k];
                            game->board[i][k] = 0;
                            k--;
                            moved = 1;
                        }
                        if (k > 0 && game->board[i][k-1] == game->board[i][k]) {
                            game->board[i][k-1] *= 2;
                            game->score += game->board[i][k-1];
                            game->board[i][k] = 0;
                            moved = true;
                            k++;
                        }
                    }
                }
            }
            break;
        case RIGHT:
            for (i = 0; i < tam; i++) {
                for (j = tam - 2; j >= 0; j--) {
                    if (game->board[i][j] != 0) {
                        k = j;
                        while (k < tam - 1 && game->board[i][k+1] == 0) {
                            game->board[i][k+1] = game->board[i][k];
                            game->board[i][k] = 0;
                            k++;
                            moved = 1;
                        }
                        if (k < tam - 1 && game->board[i][k+1] == game->board[i][k]) {
                            game->board[i][k+1] *= 2;
                            game->score += game->board[i][k+1];
                            game->board[i][k] = 0;
                            moved = true;
                            k--;
                        }
                    }
                }
            }
            break;
    }

    if (moved) {
        add_random_number(game);
        game->moves++;
    }
    else {
        printf("Movimento inválido!\n");
    }
}

bool check_win(const Game *game) {
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if (game->board[i][j] == WIN_SCORE) {
                return true;
            }
        }
    }
    return false;
}

int can_move(const Game *game) { 
    if (!check_full(game)) {
        return 1;
    }
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if ((i + 1 < tam && game->board[i][j] == game->board[i+1][j]) ||
                (j + 1 < tam && game->board[i][j] == game->board[i][j+1])) {
                return 1;
            }
        }
    }
    return 0; // Não há mais jogadas possíveis
}

int menu() {
    int opcao;
    printf("Bem-vindo ao 2048!\n");
    printf("1. Novo Jogo\n");
    printf("2. Carregar Jogo\n");
    printf("3. Sair\n");
    printf("Escolha uma opção: ");
    
    while (scanf("%d", &opcao) != 1 || opcao < 1 || opcao > 3) {
        clear_input_buffer();
        printf("Opção inválida! Escolha 1, 2 ou 3: ");
    }
    return opcao;
}

void clear_input_buffer() {
    while (getchar() != '\n');
}

int main() {
    srand(time(0));
    Game game;
    int opcao;
    
    // Carrega o highscore antes de qualquer coisa
    load_highscore(&game.highscore);  // Certifique-se de que game.highscore seja usado diretamente aqui
    
    opcao = menu();
    switch(opcao) {
        case 1:
            init_board(&game);
            break;
        case 2:
            load_game(&game, "savegame.txt");
            break;
        case 3:
            printf("Saindo...\n");
            return 0;
        default:
            break;
    }

    while (!game.game_over) {
        print_board(&game);
        printf("Pressione 'b' para salvar, z para desfazer jogada o jogo ou W, A, S, D para mover: ");
        
        char move;
        scanf(" %c", &move);

        int game_saved=0;

        if (move == 'b' || move == 'B') {
            if(!game_saved){
                save_game(&game, "savegame.txt");
                printf("Jogo salvo com sucesso!\n");
                printf("\n");
                game_saved=1;
            }
            continue;  // Pula a parte de movimento e continua o jogo
        }
        if (move == 'z' || move == 'Z') {
            undo_move(&game);  // Desfaz a última jogada
            continue;   // Pula a parte de movimento e continua o jogo
        }
        Direction dir;
        switch (move) {
            case 'w': 
                move_board(&game, UP);
                break;
            case 's': 
                move_board(&game, DOWN);
                break;
            case 'a': 
                move_board(&game, LEFT);
                break;
            case 'd': 
                move_board(&game, RIGHT);
                break;
            case 'z':
                backup(&game);
                break;
            default:
                printf("\nEntrada inválida! Use W, A, S, D.\n");

                continue;
        }
        
        // Atualiza o score e o highscore se necessário
        update_score(game.score, &game);

        if (check_win(&game)) {
            printf("Você ganhou! Parabéns!\n");
            printf("\n");
            break;
        }

        if (can_move(&game) == 0) {
            printf("Fim de jogo!\n");
            printf("\n");
            game.game_over = true;
        }
    }

    // Salva o jogo e o highscore ao final
    save_game(&game, "savegame.txt");
    save_highscore(&game); // Salva o highscore atualizado
    free_game(&game);

    return 0;
}
