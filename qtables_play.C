#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <exception>
#include <iostream>
#include <algorithm>

#include <stdlib.h>
#include <time.h>

#include <tictacto_common.h>
#include <qtable.h>


bool in_play = true;

Qtable my_qtable;

unsigned int game_board = 0;

unsigned int square(int index) {
  return (game_board >> (index * 2)) & 3;
}
void set_square(int index, std::string nval_str) {
  unsigned int nval = (nval_str == "X") ? X : O;
  int binx = index * 2;
  game_board = game_board | (nval << binx);
}
std::string sqval(int index) {
  if (square(index)==X) return "X";
  if (square(index)==O) return "O";
  return " ";
}
void display() {
    std::cout << "   " << sqval(0) << "|" << sqval(1) << "|" << sqval(2) << "\n";
    std::cout << "    + + \n";
    std::cout << "   " << sqval(3) << "|" << sqval(4) << "|" << sqval(5) << "\n";
    std::cout << "    + + \n";
    std::cout << "   " << sqval(6) << "|" << sqval(7) << "|" << sqval(8) << std::endl;
}

void get_computers_move() {
  std::cout << "board state before computers move: 0x" << std::hex << game_board << std::dec << std::endl;
  
  std::vector<unsigned int> actions;

  my_qtable.GetActions(actions,game_board);

  std::cout << "\nactions:\n";

  int best_action = -1.0;
  float high_q = 0.0;
  
  for (auto a_iter = actions.begin(); a_iter != actions.end(); a_iter++) {
    int next_action = *a_iter;
    float next_action_q = my_qtable.GetActionBias(game_board,(*a_iter));
    int move_index = next_action >> 2;
    std::string move_side = ((next_action & 3) == 2) ? "X" : "O";
    std::cout << "   action : 0x" << std::hex << next_action << std::dec << " bias: " << next_action_q
	      << " index: " << move_index << " side: " << move_side << "\n";
    if (best_action == -1.0) {
      best_action = next_action;
      high_q = next_action_q;
    } else if (next_action_q > high_q) {
      best_action = next_action;
      high_q = next_action_q;
    }
  }

  std::cout << " best action: 0x" << std::hex << best_action << std::dec << " high-Q: "
	    << high_q << "\n";

  int move_index = best_action >> 2;
  std::string move_side = ((best_action & 3) == 2) ? "X" : "O";

  std::cout << " move index/side: " << move_index << "/" << move_side << "\n"; 
  std::cout << std::endl;
  
  set_square(move_index,move_side);

  std::cout << "board state after computers move: 0x" << std::hex << game_board << std::dec << std::endl;
}

void play_a_game() {
  game_board = 0;
  
  std::cout << "X or O?";
  std::string line;
  std::getline(std::cin, line);
  std::cout << "user chose: " << line << std::endl;

  if (line == "O") {
    get_computers_move();
  }
  
  int outcome = UNKNOWN;

  while(outcome == UNKNOWN) {
    display();
    std::cout << "Your move?";
    std::string line;
    std::getline(std::cin, line);
    int board_index = 0;
    sscanf(line.c_str(),"%d",&board_index);
    std::cout << "user chose: " << board_index << std::endl;
    set_square(board_index,"O");
    display();
    get_computers_move();
  }
}


int main(int argc, char **argv) {
  std::cout << "Dude!" << std::endl;

  std::string qtable_file = "ttt_qtable_data.xml";
  
  my_qtable.ReadQtableFile(qtable_file);

  // start with initial board state,
  // choose sides

  // if computer side == X, choose starting move (best Q/action from starting board state)

  // repeat til win or draw:
  //   display board, wait for opponents move
  //   check for (opponents) win, or draw
  //   choose next move (best Q/action from starting board state)

  while(in_play) {
    play_a_game();
  }
  
  return 0;
}
