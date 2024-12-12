#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h> //setas teclado 

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
    add_random_number(game);
    add_random_number(game);
}


int menu(Game *game) {
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
}

void clear_input_buffer() {
    while (getchar() != '\n');
}

int main() {
    srand(time(0));   // inicializa o gerador de números pseudo-aleatórios
    Game game;
    game.winscore_reached = false; //confere se o valor da pontuação maximo foi atingido

    load_highscore(&game.highscore);   // Carrega o highscore antes de qualquer coisa
    
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
                menu(game);
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