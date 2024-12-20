#include <ncurses.h>

int main() {
    initscr();            // Inicializa o ncurses
    cbreak();             // Desativa o buffer de linha
    noecho();             // Desativa a exibição dos caracteres digitados
    keypad(stdscr, TRUE); // Habilita a captura das teclas de setas

    printw("Pressione qualquer tecla para continuar...");
    refresh();
    getch(); // Espera pela tecla pressionada
    endwin(); // Finaliza o ncurses

    return 0;
}
