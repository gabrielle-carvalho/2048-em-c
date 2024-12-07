#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<stdbool.h>
#include <unistd.h>  // Para o delay

//abrir jogo antigo
//nao printar um novo a cada vez
//interface melhor
//alocaçao dinamica pra salvar a maior pontuaçao a longo prazo
//usar mais ponteiros durante o codigo
//reduzir as funçoes de mover para uma, passando a direção como parametro
//colocar mais interaçoes do usuario, principalmente quando algo da errado
//delay entre as jogadas
//capturar as teclas de mover além do a,w,s,d
//opção pra abrir um jogo novo de forma mais clara, colocar menu, falar que so pode abri um novo apos fechar o aberto e pedir pra confirmar
// Move counter
// Undo move
// Bônus se fizer muitas combinações
// Opção de continuar depois de 2048?
// set_difficulty(): Alterar a dificuldade (ex.: tempo de resposta ou número inicial de peças).
// Adicionar sistema de combos: Pontuação extra por combinações consecutivas.
//(usar operações de leitura e escrita em arquivos, como fopen(), fprintf(), fscanf()).
//SE FOR PRA UM LADO E NADA MUDAR, NAO ADD RANDOM
// 5. Falta de Verificação para Pontuação Máxima
// No momento, o código só verifica se o tabuleiro contém um valor 2048 e termina o jogo. Seria interessante dar mais feedback ao jogador caso ele atinja a maior pontuação possível (ex.: quando ele atinge 2048) e adicionar a possibilidade de continuar após atingir esse valor.

#define MAXPREVGAME 50
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
} Game;
// Game game = {NULL, 0, 0, 0 };

void init_board(Game *game);
void free_game(Game *game);
void print_board(const Game *game);
void add_random_number(Game *game);
void move_board(Game *game, Direction dir);
void save_game(const Game *game, const char *filename);
void load_game(Game *game, const char *filename);
int can_move(const Game *game);
bool check_win(const Game *game);
bool check_full(const Game *game);
void backup_board(Game *game);
void restore_board(Game *game);
void update_score(int new_value, Game *game);
void save_highscore(const Game *game);
void load_highscore(Game *game);
int menu();

void init_board(Game *game) {
    game->board = (int **) malloc(tam*sizeof(int *));
    game->prev_board = (int **)malloc(tam * sizeof(int *));
    for(int i=0; i<tam; i++){
        game->board[i] = (int *)malloc(tam * sizeof(int));
        game->prev_board[i] = (int *)malloc(tam * sizeof(int));
    }
    game->score = 0;
    game->moves = 0;
    game->game_over = false;
    // for (int i = 0; i < tam; i++) {
    //     for (int j = 0; j < tam; j++) {
    //         game->board[i][j] = 0;
    //     }
    // }
    add_random_number(game);
    add_random_number(game);
}

void free_game(Game *game) {
    for (int i = 0; i < tam; i++) {
        free(game->board[i]);
        free(game->prev_board[i]);
    }
    free(game->board);
    free(game->prev_board);
}

void backup_board(Game *game){
    for (int i = 0; i < tam; i++){
        memcpy(game->prev_board[i], game->board[i], tam * sizeof(int));
    }
}

void restore_board(Game *game){
    for (int i = 0; i < tam; i++){
        memcpy(game->prev_board[i], game->board[i], tam * sizeof(int));
    }
}



bool check_full (const Game *game){
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if (game->board[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

void add_random_number(Game *game){
    if(check_full(game)){
        return;
    }
    int i, j;
    do{
        i=rand() % tam;
        j=rand() % tam;
    } 
    while(game->board[i][j] != 0); //verifica se a posicao aleatoria esta ocupada
        
     //se tiver escolhe outra posição
    game->board[i][j] = (rand() % 10 < 8) ? 2 : 4;
    // bota número aleatório (80% para 2, 20% para 4)
} //verificar se o valor aleatório gerado corresponde ao número de posições não ocupadas.

void print_board(const Game *game) {
    printf("--------------------------------\n");
    for (int i = 0; i < tam; i++) {
        printf("|");
        for (int j = 0; j < tam; j++) {
            printf(" %4d ", game->board[i][j]);
        }
        printf("|\n");
    }
    printf("--------------------------------\n");
    printf("Score: %d | Highscore: %d | Moves: %d\n", game->score, game->highscore, game->moves);
}



void save_game(const Game *game, const char *filename){
    FILE *file =fopen("savegame.txt", "w");
    if (file == NULL){
        perror("erro ao salvar jogo");
        return;
    }
    fwrite(&game->score, sizeof(int), 1, file);
    fwrite(&game->highscore, sizeof(int), 1, file);
    for(int i = 0; i< tam; i++){
        fwrite(game->board[i], sizeof(int), tam, file);
    }
    fclose(file);
    printf("Jogo salvo com sucesso!\n");
}

void load_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erro ao carregar o jogo");
        return;
    }
    fread(&game->score, sizeof(int), 1, file);
    fread(&game->highscore, sizeof(int), 1, file);
    for (int i = 0; i < tam; i++) {
        fread(game->board[i], sizeof(int), tam, file);
    }
    fclose(file);
    printf("Jogo carregado com sucesso!\n");
}

void update_score(int new_value, Game *game){
    game->score += new_value;
    if(game->score > game->highscore){
        game->highscore = game->score;
    }
}

void save_highscore(const Game *game){
    FILE *file = fopen("highscore.txt", "w");
    if(!file){
        perror("Erro ao salvar highscore");
        return;
    }
    fprintf(file, "%d\n", game->highscore);
    fclose(file);
}

void load_highscore(Game *game) {
    FILE *file = fopen("highscore.txt", "r");
    if (!file) {
        game->highscore = 0;
        return;
    }
    fscanf(file, "%d", &game->highscore);
    fclose(file);
}

void move_board(Game *game, Direction dir){
    int i, j;
    int moved = 0;
    int **prev_board = (int **) malloc(tam * sizeof(int *));
    for (int i = 0; i < tam; i++) {
        prev_board[i] = (int *) malloc(tam * sizeof(int));
    }

    switch (dir) {
        case UP:
            for (j = 0; j < tam; j++) {
                for (i = 1; i < tam; i++) {
                    if (game->board[i][j] != 0) {
                        int k = i;
                        while (k > 0 && game->board[k-1][j] == 0) {
                            game->board[k-1][j] = game->board[k][j];
                            game->board[k][j] = 0;
                            k--;
                            moved = 1;
                        }
                        if (k > 0 && game->board[k-1][j] == game->board[k][j]) {
                            game->board[k-1][j] *= 2;
                            game->score += game->board[k-1][j];
                            game->board[k][j] = 0;
                            moved = 1;
                        }
                    }
                }
            }
            break;
        case DOWN:
            for (j = 0; j < tam; j++) {
                for (i = tam - 2; i >= 0; i--) {
                    if (game->board[i][j] != 0) {
                        int k = i;
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
                            moved = 1;
                        }
                    }
                }
            }
            break;
        case LEFT:
            for (i = 0; i < tam; i++) {
                for (j = 1; j < tam; j++) {
                    if (game->board[i][j] != 0) {
                        int k = j;
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
                            moved = 1;
                        }
                    }
                }
            }
            break;
        case RIGHT:
            for (i = 0; i < tam; i++) {
                for (j = tam - 2; j >= 0; j--) {
                    if (game->board[i][j] != 0) {
                        int k = j;
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
                            moved = 1;
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
    printf("Movimento inválido! Nenhuma mudança no tabuleiro.\n");
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




int can_move(const Game *game){ 
    if(!check_full(game)){
        return 1;
    }
    for(int i = 0; i<tam; i++){
        for (int j=0;j<tam;j++){
            if ((i + 1 < tam && game->board[i][j] == game->board[i+1][j]) ||
                (j + 1 < tam && game->board[i][j] == game->board[i][j+1])) {
                return 1;
            }
        }
    }
    return 0; // não há mais jogadas entao acaba
}

int menu() {
    int opcao;
    printf("Bem-vindo ao 2048!\n");
    printf("1. Novo Jogo\n");
    printf("2. Carregar Jogo\n");
    printf("3. Sair\n");
    printf("Escolha uma opção: ");
    
    while (scanf("%d", &opcao) != 1 || opcao < 1 || opcao > 3) {
        // Limpa o buffer de entrada caso o usuário digite algo inválido
        while (getchar() != '\n'); // Limpa o buffer
        printf("Entrada inválida! Escolha uma opção válida (1, 2 ou 3): ");
    }
}


int main(){

    srand(time(NULL));
    Game game;
    load_highscore(&game);

    int opcao = menu();
    switch (opcao) {
    case 1:
        init_board(&game);
        break;
    case 2:
        load_game(&game, "savegame.txt");
        break;
    case 3:
        printf("Saindo do jogo...\n");
        return 0;
    }

    char move;

    while (1) {
        print_board(&game);
        if (check_win(&game)) {
            printf("Você atingiu 2048! Parabéns!\n");
        }
        if (!can_move(&game)) {
            printf("Jogo encerrado! Não há mais movimentos possíveis.\n");
            break;
        }

        printf("Escolha uma opção:\n");
        printf("W: Cima, A: Esquerda, S: Baixo, D: Direita, P: Salvar, L: Carregar, U: Undo, Q: Sair\n");
        scanf(" %c", &move);

        Direction dir;
        switch (move) {
            case 'w': dir = UP; move_board(&game, dir); break;
            case 'a': dir = LEFT; move_board(&game, dir); break;
            case 's': dir = DOWN; move_board(&game, dir); break;
            case 'd': dir = RIGHT; move_board(&game, dir); break;
            case 'p': save_game(&game, "savegame.txt"); break;
            case 'l': load_game(&game, "savegame.txt"); break;
            case 'u': restore_board(&game); break;
            case 'q': 
                free_game(&game);
                save_highscore(&game);
                printf("Saindo do jogo...\n");
                return 0;
            default:
                printf("Comando inválido!\n");
        }
    }
        save_highscore(&game);
        free_game(&game);
        return 0;
}
