Funcionalidades básicas
move_left()
move_right()
move_up()
move_down()
add_random_number()
combine_tiles(): Função para combinar peças de mesmo valor quando elas se encontram
check_game_over(): Função para verificar se o jogo acabou sem movimentos possíveis
check_win_condition(): Função para verificar se o jogador atingiu 2048


Interface e Exibição
print_board(): Função para exibir o tabuleiro com os valores das peças.
Adicionar bordas ao tabuleiro. Melhorar o layout e formatação das células. Adicionar cores?

    
Pontuação e Fim de Jogo
update_score(): Função para atualizar e exibir a pontuação.
game_over_message(): Exibir mensagem de "Game Over".
win_message(): Exibir mensagem de "You Win!" quando atingir 2048.


 Funções de Salvamento e Carregamento
save_game(): Função para salvar o estado do jogo em um arquivo.
load_game(): Função para carregar o estado do jogo de um arquivo salvo.

  Controle de Entrada
get_input(): Função para capturar a entrada do usuário (teclas para mover as peças).
verificação de saída pressionando Q para sair


  Melhorias na Lógica
Verificar combinação de peças após cada movimento (evitar movimentos redundantes).
Garantir que as peças não se movam para posições incorretas após cada movimento.


  Extras 
undo_move(): Função para desfazer o último movimento (se possível, usando um histórico de movimentos).
set_difficulty(): Alterar a dificuldade (ex.: tempo de resposta ou número inicial de peças).
Adicionar sistema de combos: Pontuação extra por combinações consecutivas.

