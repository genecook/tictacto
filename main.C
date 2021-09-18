#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <exception>
#include <iostream>

#include <time.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

enum SQUARE { O=1, X=2, FREE=0 };

// used when recording games:

struct game_record {
  game_record() {};
  game_record(int _side, bool _is_win, unsigned long long _moves) : side(_side),is_win(_is_win),moves(_moves) {};
  
  int side;   // O=1, X=2
  bool is_win; // true for win, else draw
  unsigned long long moves; // see tictacto_games_generator.record_move method
};

class tictacto_games_generator {
public:
  tictacto_games_generator() : board(0), side(X), moves(0), duplicate_games(0), X_wins(false), O_wins(false),
			       its_a_draw(false), wins_for_X(0), wins_for_O(0), draws(0),shortest_game(99),
			       longest_game(0) {};

  void init_for_next_game() {
    board = 0;
    moves = 0;
    moves_for_replay.erase(moves_for_replay.begin(),moves_for_replay.end());
    X_wins = false;
    O_wins = false;
    its_a_draw = false;
  };
  
  unsigned int opponent(unsigned int side) {
    if (side == X)
      return O;
    return X;
  };

  void record_move(int index, unsigned int side) {
    unsigned int next_move = (index << 2) | side;
    moves = (moves << 6) | next_move;
    moves_for_replay.push_back(next_move);
  };

  void replay_game() {
    board = 0;
    for (auto mi = moves_for_replay.begin(); mi != moves_for_replay.end(); mi++) {
      unsigned int index = (*mi) >> 2;
      unsigned int nval = (*mi) & 3;
      set_square(index,nval);
      display();
      std::cout << "\n";
    }
  };
  
  void record_game() {
    if (unique_games.find(moves) == unique_games.end()) {
      struct game_record outcome(side,!its_a_draw,moves);
      unique_games[moves] = outcome;
      if (X_wins) wins_for_X++;
      if (O_wins) wins_for_O++;
      if (its_a_draw) draws++;  
    } else
      duplicate_games++;

    if (moves_for_replay.size() < shortest_game)
      shortest_game = moves_for_replay.size();
    else if (moves_for_replay.size() > longest_game)
      longest_game = moves_for_replay.size();
  };

  int num_unique_games() {
    return unique_games.size();
  };

  int num_duplicate_games() {
    return duplicate_games;
  };
  
  unsigned int square(int index) {
    return (board >> (index * 2)) & 3;
  };

  void set_square(int index, unsigned int nval) {
    int binx = index * 2;
    board = (board | (3 << binx)) ^ (3 << binx) | (nval << binx);
  };

  bool win(unsigned int side) {
    // wins horizontal...
    if ( square(0) == square(1) && square(1) == square(2) && square(0) == side ) return true;
    if ( square(3) == square(4) && square(4) == square(5) && square(3) == side ) return true;
    if ( square(6) == square(7) && square(7) == square(8) && square(6) == side ) return true;
    // wins vertical...
    if ( square(0) == square(3) && square(3) == square(6) && square(0) == side ) return true;
    if ( square(1) == square(4) && square(4) == square(7) && square(1) == side ) return true;
    if ( square(2) == square(5) && square(5) == square(8) && square(2) == side ) return true;
    // wins diagonal...
    if ( square(0) == square(4) && square(4) == square(8) && square(0) == side ) return true;
    if ( square(2) == square(4) && square(4) == square(6) && square(2) == side ) return true;
  
    return false;
  };

  bool must_block(int &block_square, unsigned int side, bool or_win = false) {
    // blocks horizontal...

    int which_side = or_win ? side : opponent(side);
    
    if ( square(0) == square(1) && square(0) == which_side && square(2) == FREE) { block_square = 2; return true; }
    if ( square(1) == square(2) && square(1) == which_side && square(0) == FREE) { block_square = 0; return true; }
    if ( square(0) == square(2) && square(0) == which_side && square(1) == FREE) { block_square = 1; return true; }

    if ( square(3) == square(4) && square(3) == which_side && square(5) == FREE) { block_square = 5; return true; }
    if ( square(4) == square(5) && square(4) == which_side && square(3) == FREE) { block_square = 3; return true; }
    if ( square(3) == square(5) && square(3) == which_side && square(4) == FREE) { block_square = 4; return true; }

    if ( square(6) == square(7) && square(6) == which_side && square(8) == FREE) { block_square = 8; return true; }
    if ( square(7) == square(8) && square(7) == which_side && square(6) == FREE) { block_square = 6; return true; }
    if ( square(6) == square(8) && square(6) == which_side && square(7) == FREE) { block_square = 7; return true; }

    // blocks vertical...

    if ( square(0) == square(3) && square(0) == which_side && square(6) == FREE) { block_square = 6; return true; }
    if ( square(3) == square(6) && square(3) == which_side && square(0) == FREE) { block_square = 0; return true; }
    if ( square(0) == square(6) && square(0) == which_side && square(3) == FREE) { block_square = 3; return true; }

    if ( square(1) == square(4) && square(1) == which_side && square(7) == FREE) { block_square = 7; return true; }
    if ( square(4) == square(7) && square(4) == which_side && square(1) == FREE) { block_square = 1; return true; }
    if ( square(1) == square(7) && square(1) == which_side && square(4) == FREE) { block_square = 4; return true; }

    if ( square(2) == square(5) && square(2) == which_side && square(8) == FREE) { block_square = 8; return true; }
    if ( square(5) == square(8) && square(5) == which_side && square(2) == FREE) { block_square = 2; return true; }
    if ( square(2) == square(8) && square(2) == which_side && square(5) == FREE) { block_square = 5; return true; }

    // blocks diagonal...

    if ( square(0) == square(4) && square(0) == which_side && square(8) == FREE) { block_square = 8; return true; }
    if ( square(4) == square(8) && square(4) == which_side && square(0) == FREE) { block_square = 0; return true; }
    if ( square(0) == square(8) && square(0) == which_side && square(4) == FREE) { block_square = 4; return true; }

    if ( square(2) == square(4) && square(2) == which_side && square(6) == FREE) { block_square = 6; return true; }
    if ( square(4) == square(6) && square(4) == which_side && square(2) == FREE) { block_square = 2; return true; }
    if ( square(2) == square(6) && square(2) == which_side && square(4) == FREE) { block_square = 4; return true; }

    return false;
  };

  bool claim_win(int &block_square, unsigned int side) {
    return must_block(block_square, side, true);
  }
  
  bool draw() {
    // will ASSUME (for now) that game is not draw if any square is free...
    for (int i = 0; i < 9; i++) {
      if (square(i) == FREE)
	return false;
    }
    return true;
  };

  std::string sqval(int index) {
    if (square(index)==X) return "X";
    if (square(index)==O) return "O";
    return " ";
  };
  
  void display() {
    std::cout << "   " << sqval(0) << "|" << sqval(1) << "|" << sqval(2) << "\n";
    std::cout << "    + + \n";
    std::cout << "   " << sqval(3) << "|" << sqval(4) << "|" << sqval(5) << "\n";
    std::cout << "    + + \n";
    std::cout << "   " << sqval(6) << "|" << sqval(7) << "|" << sqval(8) << std::endl;  
  };

  unsigned int random_game(bool display_outcome = false) {
    init_for_next_game();

    bool game_over = false;

    side = X; // X always goes first...
  
    while(!game_over) {
      int ns;
      if (must_block(ns,side)) {
	// block win for opponent...
      } else if (claim_win(ns,side)) {
	// this square wins...
      } else
	ns = (rand() & 0xf) % 9;
      if (square(ns) == FREE) {
	set_square(ns, side);
	record_move(ns, side);
	if (win(side)) {
	  game_over = true;
	  if (side==X)
	    X_wins = true;
	  else
	    O_wins = true;
	  if (display_outcome) std::cout << "WIN FOR " << (side==X ? "X" : "O") << "\n";
	} else if (side == X)
	  side = O;
	else
	  side = X;
	if (!game_over && draw()) {
	  game_over = true;
	  its_a_draw = true;
	  if (display_outcome) std::cout << "DRAW\n";
	}
      }
    }
  
    return moves;
  };

  int num_wins_X() { return wins_for_X; };
  int num_wins_O() { return wins_for_O; };
  int num_draws()  { return draws; };

  int the_shortest_game() { return shortest_game; };
  int the_longest_game() { return longest_game; };
  
private:
  unsigned int board;
  int side;
  unsigned long long moves;
  std::vector<unsigned int> moves_for_replay;
  std::map<unsigned long long, struct game_record> unique_games;
  int duplicate_games;
  bool X_wins;
  bool O_wins;
  bool its_a_draw;
  int wins_for_X;
  int wins_for_O;
  int draws;  
  int shortest_game;
  int longest_game;    
};

int main(int argc, char **argv) {
  std::cout << "Generating random tic-tac-to games..." << std::endl;

  tictacto_games_generator my_generator;
  
  for (int i = 0; i < 10000000; i++) {
    my_generator.random_game();
    my_generator.record_game();
    //my_generator.replay_game();
  }
  
  std::cout << "# of unique games:    " << my_generator.num_unique_games() << "\n";
  std::cout << "# of duplicate games: " << my_generator.num_duplicate_games() << "\n";
  std::cout << "# of wins for X:      " << my_generator.num_wins_X() << "\n";
  std::cout << "# of wins for O:      " << my_generator.num_wins_O() << "\n";
  std::cout << "# of draws:           " << my_generator.num_draws() << std::endl;
  std::cout << "shortest game:        " << my_generator.the_shortest_game() << std::endl;
  std::cout << "longest game:         " << my_generator.the_longest_game() << std::endl;
    

}

