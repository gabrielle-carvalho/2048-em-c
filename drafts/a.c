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

void init_board(Game *game), free_game(Game *game), print_board(const Game *game), backup(Game *game), update_score(int new_value, Game *game), save_highscore(const Game *game), add_random_number(Game *game), move_board(Game *game, Direction dir), save_game(Game *game, const char *filename), load_game(Game *game, const char *filename), load_highscore(int *highscore), clear_input_buffer(); 
int can_move(const Game *game), menu();
bool check_win(const Game *game), check_full(const Game *game);

void init_board(Game *game) {
//aloca o array
    game->board = (int **) malloc(tam * sizeof(int *));
    game->prev_board = (int **) malloc(tam * sizeof(int *));
    game->backup_board = malloc(tam * sizeof(int *));
    if (!game->board || !game->prev_board || !game->backup_board) {
        perror("Falha na alocação de memória.");
        return;
    }
//aloca cada posição do array
    for (int i = 0; i < tam; i++) {
        game->board[i] = (int *) malloc(tam * sizeof(int));
        game->prev_board[i] = (int *) malloc(tam * sizeof(int));
        game->backup_board[i] = malloc(tam * sizeof(int));
        if (!game->board[i] || !game->prev_board[i] || !game->backup_board[i]) {
            perror("Falha na alocação de memória para uma linha.");
            return;
        }
    }
    game->score = 0;
    game->moves = 0;
    game->game_over = false;
    game->backup_saved = false;
    // game->backup_board = (int **) malloc(tam * sizeof(int *));
    // for(int i=0; i<tam; i++){
    //     game->backup_board[i] = (int *) malloc(tam * sizeof(int));
    // }
    add_random_number(game);
    add_random_number(game);
}

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

    //game->backup_saved = true;    // Garantir que o backup_saved seja verdadeiro após carregar o jogo
//
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
    menu();
    return 0; // não há mais jogadas possíveis
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
    srand(time(0));   // inicializa o gerador de números pseudo-aleatórios
    Game game;
    game.winscore_reached = false; //confere se o valor da pontuação maximo foi atingido
    int opcao;
    load_highscore(&game.highscore);   // Carrega o highscore antes de qualquer coisa

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

    while (1) {
        print_board(&game);
        
        if (can_move(&game) == 0) {
            printf("Fim de jogo!\n");
            printf("Pontuação Máxima: %d\n", game.score);
            game.game_over = true;
            break;
        }

        if (check_win(&game)&& !game.winscore_reached) {
            game.winscore_reached = true;
            printf("Você ganhou! Parabéns!\n");
            printf("Pontuação Final: %d\n", game.score);

            char escolha;
            do {
                printf("Deseja continuar jogando? J para continuar, K para encerrar\n");
                scanf(" %c", &escolha);
                if (escolha == 'J' || escolha == 'j') {
                    printf("Continuando o jogo!\n");
                    break; // Sai do loop, jogo continua.
                } else if (escolha == 'K' || escolha == 'k') {
                    printf("Jogo finalizado. Obrigado por jogar!\n");
                    game.game_over = true; // Finaliza o jogo.
                    break;
                } else {
                    printf("Entrada inválida. Tente novamente.\n");
                }
            } while (true);

            if (game.game_over) {
                printf("Fim de jogo!\n");
                printf("Pontuação Máxima: %d\n", game.score);
                game.game_over = true;
                menu();
                break; // Encerra o loop principal do jogo.
            }
        }

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
                printf("\nEntrada inválida! Use W, A, S, D. Ou Z para desfazer\n");
                continue;
        }    
        update_score(game.score, &game);// Atualiza o score e o highscore
    }
    save_game(&game, "savegame.txt");// Salva o jogo ao final
    save_highscore(&game); // Salva o highscore atualizado
    free_game(&game); //libera a memoria
    return 0;
}