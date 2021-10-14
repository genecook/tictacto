#ifndef QTABLE_GENERATOR_INCLUDES

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


class tictacto_qtable_generator {
public:
  tictacto_qtable_generator() {};
  tictacto_qtable_generator(std::string qtable_file,std::string games_file) {
    read_games_files(games_file);

    std::cout << "# of Qtable states: " << my_qtable.StatesCount() << "\n";
    std::cout << "Highest Qtable state action count: " << my_qtable.HighActionCount() << std::endl;
    
    write_qtable_file(qtable_file);
  };

  unsigned long long encode_move(unsigned long long moves, int index, unsigned int side) {
    unsigned int next_move = (index << 2) | side;  // encode move: board square index + side
    return (moves << 6) | next_move;               // moves then will include all moves made during some game
  };

  struct move {
    move() {};
    move(int _board_index, int _side) : board_index(_board_index), side(_side) {};
    int board_index;
    int side;
  };

  void extract_states(int game_number, unsigned long long encoded_moves, bool is_win,
		      std::vector<struct move> &moves);

  void read_games_files(std::string &games_file);

  void write_qtable_file(std::string &qtable_file) {
    my_qtable.WriteQtableFile(qtable_file);
  };
  
private:
  Qtable my_qtable;
};

#endif
#define QTABLE_GENERATOR_INCLUDES 1

