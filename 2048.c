#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define MAXPREVGAME 50  
// mudar pra alocação dinamica

typedef struct {
    int board[4][4];
    int score;
    int highscore;
    int moves;
} Game;

Game game = { { {0} }, 0, 0, 0 };


void addrandomnumber(int tam, int board[tam][tam]){
    int number;

    int i=0, j=0;
    while(board[i][j] != 0){ //verifica se a posicao aleatoria esta ocupada
        i=rand() % tam;
        j=rand() % tam;
    } //se tiver escolhe outra posição

    board[i][j] = (rand() % 10 < 8) ? 2 : 4;
    // bota número aleatório (80% para 2, 20% para 4)
}

void resetgame(int tam, int board[tam][tam], int score){
    int i, j;
    for(i=0; i<tam; i++){
        for(j=0; j<tam; j++){
            board[i][j] = 0;
        }
    }
    system("clear || cls");
    score = 0;
    addrandomnumber();
}



int findlen(int n){ //recebe de QUEM
    int len = (n==0) ? 1 : 0; // atribui a len o valor dependendo do de n
    while(n>0){
        len++;
        n/=10;
    }
    return len;
}

void move_left(int tam, int board[tam][tam]){
    for (int i =0; i<tam;i++){
        int position = 0;
        for(int j=0; j<tam; j++){
            if(board[i][j] != 0){
                if(position> 0 && board[i][position-1] == board[i][j]){ //verifica se o valor é igual ao da esquerda
                    board[i][position-1] *= 2; //mutiplica o valor do lado do movimento por 2
                    board[i][j]=0; //apaga o valor que tava do lado oposto ao movimento
                }
                else{
                    if(j != position){ //posicao mais a erquerda possivel, nao deixando espaços vazios
                        board[i][position] = board[i][j];
                        board[i][j] = 0;
                    }
                    position++;
                }
            }
        }
    }
}

void move_right(int tam, int board[tam][tam]){
    for (int i =0; i<tam;i++){
        int position = 0;
        for(int j=0; j<tam; j++){
            if(board[i][j] != 0){
                if(position> 0 && board[i][position+1] == board[i][j]){ //verifica se o valor é igual ao da esquerda
                    board[i][position+1] *= 2; //mutiplica o valor do lado do movimento por 2
                    board[i][j]=0; //apaga o valor que tava do lado oposto ao movimento
                }
                else{
                    if(j != position){ //posicao mais a erquerda possivel, nao deixando espaços vazios
                        board[i][position] = board[i][j];
                        board[i][j] = 0;
                    }
                    position++;
                }
            }
        }
    }
}

int can_move(int tam, int board[tam][tam]){  // (int tam, int board [tam][tam])
    for(int i = 0; i<tam; i++){
        for (int j=0;j<tam;j++){
            if(board[i][j] == 0){ //se tiver espaços vazios ele continua
                return 1;
            }
            if(j < (tam-1) && board[i][j] == board[i][j+1]){
                return 1;
            } // verifica se há combinações possível na horizontal
            if(i < (tam-1) && board[i][j] == board[i +1][i]){
                return 1; //verifica se há combinações possíveis na vertical
            }
        }
    }
        return 0; // não há mais jogadas entao acaba
}

void save_game(Game *game){
    FILE *file =fopen("savegame.txt", "w");
    if (file == NULL){
        perror("erro ao salvar jogo");
        return;
    }
    fwrite(game, sizeof(Game), 1, file);
    fclose(file);
}

void load_game(Game *game){
    FILE *file =fopen("savegame.txt", "r");
    if (file == NULL){
        printf("Nenhum jogo anterior encontrado");
        return;
    }
    fread(game, sizeof(Game), 1, file);
    fclose(file);
}

int main(){
    srand(time(NULL)); // inicializar o gerador de números aleatórios
    int tam = 4;
    int board[4][4] = {0};
    addrandomnumber(tam, board);
        // Exibir o tabuleiro
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            printf("%4d", board[i][j]);
        }
        printf("\n");
    }

    return 0;
}

// int arr[4][4]= {0}, score = 0, highscore = 0; stepsnumber = 0, tam=4, temparr[4], temp=0, len=0, count=0;


// //     movevalue(int k): Moves non-zero values to the rightmost side in an array.
// void movevalue(int k){
//     int i;
//     if(k==2){
//         return;
//     }
//     for (i = k; i<tam; i++){
//         if(temparr[i] != 0){
//             movevalue(i+1);
//             for(i=0; i<tam; i++){
//                 if(temparr[i+1] != 0){
//                     break;
//                 }
//                 temparr[i+1] = temparr[i];
//                 temparr[i] = 0;
//             }
//         }
//     }
// }

