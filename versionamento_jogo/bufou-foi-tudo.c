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
        printw("Nenhuma jogada para desfazer!\n");
        return;
    }
    game->score = game->backup_score;
    game->moves = game->backup_moves; // Restaurar o número de movimentos

    for (int i = 0; i < tam; i++) {
        memcpy(game->board[i], game->backup_board[i], tam * sizeof(int));// Restaura o estado do jogo a partir do backup
    }
    game->backup_saved = false;  // Resetar o estado de backup
    printw("Última jogada desfeita.\n");
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

void save_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "wb");
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
    printw("Jogo salvo em: %s", filename);
}

void load_game(Game *game, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printw("Erro ao abrir o arquivo: %s\n", filename);
        return;
    }

    // Ler o score, highscore e número de movimentos
    if (fread(&game->score, sizeof(int), 1, file) != 1 ||
        fread(&game->highscore, sizeof(int), 1, file) != 1 ||
        fread(&game->moves, sizeof(int), 1, file) != 1) {
        printw("Erro ao ler as informações do jogo.\n");
        fclose(file);
        return;
    }

    // Alocar memória para os tabuleiros
    game->board = (int **)malloc(tam * sizeof(int *));
    game->prev_board = (int **)malloc(tam * sizeof(int *));
    game->backup_board = (int **)malloc(tam * sizeof(int *));
    
    // Verificar se a alocação foi bem-sucedida
    if (!game->board || !game->prev_board || !game->backup_board) {
        printw("Erro ao alocar memória para os tabuleiros.\n");
        fclose(file);
        return;
    }

    for (int i = 0; i < tam; i++) {
        game->board[i] = (int *)malloc(tam * sizeof(int));
        game->prev_board[i] = (int *)malloc(tam * sizeof(int));
        game->backup_board[i] = (int *)malloc(tam * sizeof(int));
        
        // Verificar se a alocação de cada linha foi bem-sucedida
        if (!game->board[i] || !game->prev_board[i] || !game->backup_board[i]) {
            printw("Erro ao alocar memória para as linhas dos tabuleiros.\n");
            fclose(file);
            return;
        }
    }

    // Ler os tabuleiros
    for (int i = 0; i < tam; i++) {
        // Verificando se a leitura de cada linha do tabuleiro foi bem-sucedida
        if (fread(game->board[i], sizeof(int), tam, file) != tam) {
            printw("Erro ao ler o tabuleiro (board) da linha %d.\n", i);
            fclose(file);
            return;
        }

        if (fread(game->prev_board[i], sizeof(int), tam, file) != tam) {
            printw("Erro ao ler o tabuleiro (prev_board) da linha %d.\n", i);
            fclose(file);
            return;
        }

        if (fread(game->backup_board[i], sizeof(int), tam, file) != tam) {
            printw("Erro ao ler o tabuleiro (backup_board) da linha %d.\n", i);
            fclose(file);
            return;
        }
    }

    fclose(file);
    printw("Jogo carregado com sucesso!\n");
}

void update_score(int new_value, Game *game) {
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
    bool moved = false;
    backup(game);

    switch (dir) {
        case UP:
            for (j = 0; j < tam; j++) {
                int move = 0;
                bool merged[tam] = {false};  // inicializa todos os blocos como nao mesclados
                for (i = 1; i < tam; i++) {
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
        game->moves++;
    } else {
        printw("Movimento inválido!\n");
    }
}

bool check_win(const Game *game) {
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
            if (j + 1 < tam && game->board[i][j] == game->board[i][j + 1]) {
                return 1;  // Movimento possível à direita
            }

            // Verificar células abaixo
            if (i + 1 < tam && game->board[i][j] == game->board[i + 1][j]) {
                return 1;  // Movimento possível para baixo
            }
        }
    }
    printw("Não há mais jogadas possíveis.");
    //menu();
    return 0; // não há mais jogadas possíveis
}

void handle_input(Game *game) {
    static int game_saved = 0;  // Variável global para indicar se o jogo foi salvo
    int ch = getch(); // Obtém a tecla pressionada
    switch (ch) {
        case KEY_UP:
            move_board(game, UP);
            break;
        case KEY_DOWN:
            move_board(game, DOWN);
            break;
        case KEY_LEFT:
            move_board(game, LEFT);
            break;
        case KEY_RIGHT:
            move_board(game, RIGHT);
            break;
        case 'z':
            undo_move(game);
            printw("Última jogada desfeita.\n");
            break;
        case 'b':  // Para 'b' ou 'B'
        case 'B':
            if (!game_saved) {
                save_game(game, "savegame.txt");
                printw("Jogo salvo com sucesso!\n");
                game_saved = 1;  // Marcar que o jogo foi salvo
            } else {
                printw("Jogo já foi salvo.\n");
            }
            break;
        case 'q':
        case 'Q':
            game->game_over = true; // Finaliza o jogo se 'q' for pressionado
            break;
    }
}

int menu() {
    int choice = 0;
    int highlight = 0;
    const char *options[] = {"Iniciar Novo Jogo", "Carregar Jogo", "Sair"};
    int num_options = sizeof(options) / sizeof(options[0]);

    initscr();            // Inicializa o ncurses
    cbreak();             // Desativa o buffer de linha
    noecho();             // Desativa a exibição dos caracteres digitados
    keypad(stdscr, TRUE); // Habilita a captura das teclas de setas

    while (1) {
        clear(); // Limpa a tela a cada loop

        // Exibe as opções do menu
        for (int i = 0; i < num_options; i++) {
            if (i == highlight) {
                attron(A_REVERSE); // Destaca a opção selecionada
                printw("%s\n", options[i]);
                attroff(A_REVERSE); // Remove o destaque
            } else {
                printw("%s\n", options[i]);
            }
        }
        refresh(); // Atualiza a tela

        // Espera pela entrada do usuário
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                highlight = (highlight == 0) ? num_options - 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == num_options - 1) ? 0 : highlight + 1;
                break;
            case 10: // Enter
                endwin(); // Finaliza o ncurses
                return highlight; // Retorna a opção escolhida
        }
    }
}

void clear_input_buffer() {
    while (getchar() != '\n');  // Limpa até a próxima linha
}

void loop_jogo(Game *game) {
    while (1) {
        print_board(game);
        printw("Pressione 'b' para salvar, z para desfazer jogada ou W, A, S, D para mover: ");

        
        if (can_move(game) == 0) {
            printw("Fim de jogo!\n");
            printw("Pontuação Final: %d\n", game->score);
            printw("Pontuação Máxima: %d\n", game->highscore);
            refresh();
            game->game_over = true;
            break;
        }

        if (check_win(game) && !game->winscore_reached) {
            game->winscore_reached = true;
            printw("Parabéns! Você atingiu a pontuação de %d!\n", WIN_SCORE);
            refresh();
            printw("Deseja continuar jogando? J para continuar, K para encerrar.\n");
            refresh();

            int input = getch();
            if (input == 'k' || input == 'K') {
                game->game_over = true;  // Finaliza o jogo
                break;
            } else if (input == 'J' || input == 'j') {
                printw("Continuando jogo...\n");
            
            refresh(); 

            if (game->game_over) {
                printw("Fim de jogo!\n");
                printw("Pontuação Máxima: %d\n", game->score);
                game->game_over = true;
                break; // Encerra o loop principal do jogo.
            }
        }

        
        char move;
        scanf(" %c", &move);




        Direction dir;
        switch (move) {
            case 'w': 
                move_board(game, UP);
                break;
            case 's': 
                move_board(game, DOWN);
                break;
            case 'a': 
                move_board(game, LEFT);
                break;
            case 'd': 
                move_board(game, RIGHT);
                break;
            default:
                printw("\nEntrada inválida! Use W, A, S, D.\n");
                continue;
        }
        update_score(game->score, game); // Atualiza o score e o highscore
    }
}
}

int main() {
    
    int choice = menu();
    Game game;
    while (1) {
        switch (choice) {
            case 1:
                init_board(&game);
                load_highscore(&game.highscore);
                print_board(&game);
                handle_input(&game);
                break;

            case 2:
                load_game(&game, "savegame.txt");
                game.game_over = false;
                loop_jogo(&game);
                break;
            case 3:
                printw("Saindo...\n");
                return 0;
            default:
                break;
        }
    if (game.game_over) {
        // Quando o jogo terminar, volta para o menu
        printw("\nFim do Jogo!\n");
        //save_game(&game, "savegame.txt"); // Salva o jogo ao final
        save_highscore(&game); // Salva o highscore atualizado
        free_game(&game); // Libera a memória
        endwin();
    }
    }
    return 0;
}
