// void load_game(Game *game, const char *filename) {
//     FILE *file = fopen(filename, "rb");
//     if (file == NULL) {
//         printf("Erro ao abrir o arquivo: %s\n", filename);
//         return;
//     }

//     // If the game structure is not initialized (e.g., for a new game), allocate memory
//     if (!game->board) {
//         game->board = (int **)malloc(tam * sizeof(int *));
//         game->prev_board = (int **)malloc(tam * sizeof(int *));
//         game->backup_board = (int **)malloc(tam * sizeof(int *));

//     if (!game->board || !game->prev_board || !game->backup_board) {
//             perror("Erro ao alocar memória para os tabuleiros.\n");
//             fclose(file);
//             return;
//     } 

//     for (int i = 0; i < tam; i++) {
//             game->board[i] = (int *)malloc(tam * sizeof(int));
//             game->prev_board[i] = (int *)malloc(tam * sizeof(int));
//             game->backup_board[i] = (int *)malloc(tam * sizeof(int));

//             if (!game->board[i] || !game->prev_board[i] || !game->backup_board[i]) {
//                 perror("Erro ao alocar memória para a linha %d dos tabuleiros.\n");
//                 fclose(file);
//                 return;
//             }
//         }
//     }

//     // // Aloca memória para os tabuleiros
//     // if (!game->board) {
//     //     game->board = (int **)malloc(tam * sizeof(int *));
//     //     game->prev_board = (int **)malloc(tam * sizeof(int *));
//     //     game->backup_board = (int **)malloc(tam * sizeof(int *));

//     //     if (!game->board || !game->prev_board || !game->backup_board) {
//     //         perror("Erro ao alocar memória para os tabuleiros.\n");
//     //         fclose(file);
//     //         return;
//     //     }

//     //     for (int i = 0; i < tam; i++) {
//     //         game->board[i] = (int *)malloc(tam * sizeof(int));
//     //         game->prev_board[i] = (int *)malloc(tam * sizeof(int));
//     //         game->backup_board[i] = (int *)malloc(tam * sizeof(int));

//     //         if (!game->board[i] || !game->prev_board[i] || !game->backup_board[i]) {
//     //             fprintf(stderr, "Erro ao alocar memória para a linha %d dos tabuleiros.\n", i);
//     //             // ... (free memory as before)
//     //             fclose(file);
//     //             return;
//     //         }
//     //     }
//     // }

//     // // Lê o score, highscore e número de movimentos
//     // size_t items_read;
//     // items_read = fread(&game->score, sizeof(int), 1, file);
//     // if (items_read != 1) {
//     //     printf("Erro ao ler o score.\n");
//     //     fclose(file);
//     //     return;
//     // }

//     // items_read = fread(&game->highscore, sizeof(int), 1, file);
//     // if (items_read != 1) {
//     //     printf("Erro ao ler o highscore.\n");
//     //     fclose(file);
//     //     return;
//     // }

//     // items_read = fread(&game->moves, sizeof(int), 1, file);
//     // if (items_read != 1) {
//     //     printf("Erro ao ler o número de movimentos.\n");
//     //     fclose(file);
//     //     return;
//     // }

//     // // Lê o tabuleiro (cada linha)
//     // for (int i = 0; i < tam; i++) {
//     //     items_read = fread(game->board[i], sizeof(int), tam, file);
//     //     if (items_read != tam) {
//     //         printf("Erro ao ler a linha %d do board. Lidos %zu itens.\n", i, items_read);
//     //         fclose(file);
//     //         return;
//     //     }

//     //     items_read = fread(game->prev_board[i], sizeof(int), tam, file);
//     //     if (items_read != tam) {
//     //         printf("Erro ao ler a linha %d do prev_board. Lidos %zu itens.\n", i, items_read);
//     //         fclose(file);
//     //         return;
//     //     }

//     //     items_read = fread(game->backup_board[i], sizeof(int), tam, file);
//     //     if (items_read != tam) {
//     //         printf("Erro ao ler a linha %d do backup_board. Lidos %zu itens.\n", i, items_read);
//     //         fclose(file);
//     //         return;
//     //     }
//     // }

//     fread(&game->score, sizeof(int), 1, file);
//     fread(&game->highscore, sizeof(int), 1, file);
//     fread(&game->moves, sizeof(int), 1, file);

//     for (int i = 0; i < tam; i++) {
//         fread(game->board[i], sizeof(int), tam, file);
//         fread(game->prev_board[i], sizeof(int), tam, file);
//         fread(game->backup_board[i], sizeof(int), tam, file);
//     }

//     fclose(file);
//     printf("Jogo carregado com sucesso!\n");
//     game->backup_saved = true;
// }





// void load_game(Game *game, const char *filename) {
//     FILE *file = fopen(filename, "rb");
//     if (file == NULL) {
//         printf("Erro ao abrir o arquivo: %s\n", filename);
//         return;
//     }

//     // If the game structure is not initialized (e.g., for a new game), allocate memory
//     if (!game->board) {
//         game->board = (int **)malloc(tam * sizeof(int *));
//         game->prev_board = (int **)malloc(tam * sizeof(int *));
//         game->backup_board = (int **)malloc(tam * sizeof(int *));

//         if (!game->board || !game->prev_board || !game->backup_board) {
//             perror("Erro ao alocar memória para os tabuleiros.\n");
//             fclose(file);
//             return;
//         }

//         for (int i = 0; i < tam; i++) {
//             game->board[i] = (int *)malloc(tam * sizeof(int));
//             game->prev_board[i] = (int *)malloc(tam * sizeof(int));
//             game->backup_board[i] = (int *)malloc(tam * sizeof(int));

//             if (!game->board[i] || !game->prev_board[i] || !game->backup_board[i]) {
//                 perror("Erro ao alocar memória para a linha %d dos tabuleiros.\n");
//                 // ... (free memory as before)
//                 fclose(file);
//                 return;
//             }
//         }
//     }

//     // Read game data from the file
//     fread(&game->score, sizeof(int), 1, file);
//     fread(&game->highscore, sizeof(int), 1, file);
//     fread(&game->moves, sizeof(int), 1, file);

//     for (int i = 0; i < tam; i++) {
//         fread(game->board[i], sizeof(int), tam, file);
//         fread(game->prev_board[i], sizeof(int), tam, file);
//         fread(game->backup_board[i], sizeof(int), tam, file);
//     }

//     fclose(file);
//     printf("Jogo carregado com sucesso!\n");
//     game->backup_saved = true;
// }