void print_board(const Game *game) {
    system("clear || cls"); //nao deixa os boards acumularem
    printf("-----------------------------\n");
    for (int i = 0; i < tam; i++) {
        printf("|");
        for (int j = 0; j < tam; j++) {
            if(game->board[i][j] == 0){
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
