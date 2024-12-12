#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

int main() {
    srand(time(0)); // Inicializa o gerador de números aleatórios
    Game game;
    int game_saved = 0; //flag para verificar se o jogo foi salvo nessa sessão

    load_highscore(&game.highscore); // Carrega o highscore do arquivo

    while (1) {
        if (!game.board) { // Verifica se o tabuleiro foi inicializado
            if(menu(&game) == 0) break; //inicia o menu antes de printar qualquer coisa, caso o usuario queira sair, o loop para
        }
        print_board(&game);
        
        if (can_move(&game) == 0) {
            printf("Fim de jogo!\n");
            printf("Pontuação Máxima: %d\n", game.score);
            game.game_over = true;
            break;
        }

        if (check_win(&game) && !game.winscore_reached) {
            game.winscore_reached = true;
            printf("Você ganhou! Parabéns!\n");
            printf("Pontuação Final: %d\n", game.score);

            char escolha;
            do {
                printf("Deseja continuar jogando? (J para continuar, K para sair): ");
                scanf(" %c", &escolha);
                if (escolha == 'J' || escolha == 'j') {
                    printf("Continuando o jogo!\n");
                    break;
                } else if (escolha == 'K' || escolha == 'k') {
                    printf("Jogo finalizado. Obrigado por jogar!\n");
                    game.game_over = true;
                    break;
                } else {
                    printf("Entrada inválida. Tente novamente.\n");
                    clear_input_buffer(); // Limpa o buffer de entrada
                }
            } while (1);

            if (game.game_over) {
                break;
            }
        }

        printf("Pressione 'b' para salvar, 'z' para desfazer ou W, A, S, D para mover: ");

        char move;
        scanf(" %c", &move);
        clear_input_buffer();//limpa buffer apos pegar o char

        if (move == 'b' || move == 'B') {
            if(!game_saved){
                save_game(&game, "savegame.txt");
                game_saved = 1;
            } else {
                printf("O jogo já foi salvo nessa sessão!\n");
            }
            continue;
        }

        if (move == 'z' || move == 'Z') {
            undo_move(&game);
            continue;
        }

        Direction dir;
        switch (move) {
            case 'w':
            case 'W':
                dir = UP;
                break;
            case 's':
            case 'S':
                dir = DOWN;
                break;
            case 'a':
            case 'A':
                dir = LEFT;
                break;
            case 'd':
            case 'D':
                dir = RIGHT;
                break;
            default:
                printf("Entrada inválida! Use W, A, S, D, B ou Z.\n");
                continue;
        }

        move_board(&game, dir);
        update_score(game.score, &game);
        game_saved = 0; // Reseta a flag após um movimento válido
    }

    save_game(&game, "savegame.txt"); // Salva o jogo ao final
    save_highscore(&game);
    free_game(&game);
    return 0;
}