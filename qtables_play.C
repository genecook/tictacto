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

int computers_side = X;
int opponents_side = O;

unsigned int square(int index) {
  return (game_board >> (index * 2)) & 3;
}
void set_square(int index, int side) {
  int binx = index * 2;
  game_board = game_board | (side << binx);
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

int win_indices[8][3] = {
		       { 0, 1, 2 }, // wins horizontal...
		       { 3, 4, 5 },
		       { 6, 7, 8 },
		       { 0, 3, 6 }, // vertical...
		       { 1, 4, 7 },
		       { 2, 5, 8 },
		       { 0, 4, 8 }, // diagonal...
		       { 2, 4, 6 }
};

bool three_in_a_row(unsigned int &side, int set) {
  if ( (square(win_indices[set][0]) == square(win_indices[set][1]) ) &&
       (square(win_indices[set][1]) == square(win_indices[set][2]) ) ) {
    side = square(win_indices[set][0]);
    return true;
  }
  return false;
}

bool a_win(unsigned int side) {
  unsigned int winning_side;
  for (int i = 0; i < 9; i++) {
    if (three_in_a_row(winning_side,i) && (winning_side == side))
      return true;
  }
  return false;
}

bool a_draw() {
  for (int i = 0; i < 9; i++) {
    if (square(i) == FREE)
      return false;
  }
  return true;
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
    int side = next_action & 3;
    if (side != computers_side)
      continue;
    std::string move_side = (side == X) ? "X" : "O";
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
  
  set_square(move_index,computers_side);

  std::cout << "board state after move: 0x" << std::hex << game_board << std::dec << std::endl;
}

//*****************************************************************************************
//*****************************************************************************************

void play_a_game() {
  game_board = 0;
  
  std::cout << "X or O?";
  std::string line;
  std::getline(std::cin, line);
  std::cout << "user chose: " << line << std::endl;

  if (line == "O") {
    get_computers_move();
  } else {
    computers_side = O;
    opponents_side = X;
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
    set_square(board_index,opponents_side);
    display();
    
    if (a_win(opponents_side)) {
      outcome = WIN;
      std::cout << "YOU WON THIS GAME!" << std::endl;
      continue;
    }
    if (a_draw()) {
      outcome = DRAW;
      std::cout << "GAME IS A DRAW." << std::endl;
      continue;
    }
    
    get_computers_move();
    if (a_win(computers_side)) {
      display();
      outcome = WIN;
      std::cout << "COMPUTER WINS THIS GAME!" << std::endl;
      continue;
    }
    if (a_draw()) {
      outcome = DRAW;
      std::cout << "GAME IS A DRAW." << std::endl;
    }
  }
}

//*****************************************************************************************
//*****************************************************************************************

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
