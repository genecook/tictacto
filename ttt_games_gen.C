#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <exception>
#include <iostream>

#include <time.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

namespace pt = boost::property_tree;

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
    // reset current game state...
    board = 0;
    moves = 0;
    moves_for_replay.erase(moves_for_replay.begin(),moves_for_replay.end());
    X_wins = false;
    O_wins = false;
    its_a_draw = false;
  };
  
  unsigned int opponent(unsigned int side) { return (side == X) ? O : X; };

  void record_move(int index, unsigned int side) {
    unsigned int next_move = (index << 2) | side;  // encode move: board square index + side
    moves = (moves << 6) | next_move;       // moves then will include all moves made during some game
    moves_for_replay.push_back(next_move);  // also record individual moves for convenience when replaying current game
  };

  int move_count() { return (int) moves_for_replay.size(); };
  
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

  void write_games_file(std::string gfile) {
    pt::ptree tree;

    int game_index = 0;

    for (auto uiter = unique_games.begin(); uiter != unique_games.end(); uiter++) {
       game_index++;
       char game_node_name[128];
       sprintf(game_node_name,"games.game_%d",game_index);

       char game_id[128];
       sprintf(game_id,"0x%08llx",uiter->first);

       char tbuf[256];
       sprintf(tbuf,"%s.side",game_node_name);
       if (uiter->second.side == X)
	 tree.add(tbuf,"X");
       else
	 tree.add(tbuf,"X");
       sprintf(tbuf,"%s.outcome",game_node_name);
       if (uiter->second.is_win)
	 tree.add(tbuf,"WIN");
       else
	 tree.add(tbuf,"DRAW");
	 
       int index = 99;
       int side = FREE;

       int mcnt = 0;
       for (int mi = 9; mi >= 0; mi--) {
	 unsigned int next_move = ((uiter->second.moves) >> (6 * mi)) & 0x3f;
	 if (next_move == 0)
	   continue;
	 mcnt++;
	 int index = next_move >> 2;
	 std::string side = ((next_move & 3) == 2) ? "X" : "O";
	 char tbuf[256];
	 sprintf(tbuf,"%s.moves.move_%d.index",game_node_name,mcnt);
	 tree.add(tbuf,index);
	 sprintf(tbuf,"%s.moves.move_%d.side",game_node_name,mcnt);
	 tree.add(tbuf,side);
       }
    }

    pt::write_xml(gfile,tree);
  };

  // record current game, if it represents a unique game, based on the moves made.
  // update some game stats...
  
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

  int num_unique_games() { return unique_games.size(); };

  int num_duplicate_games() { return duplicate_games; };
  
  unsigned int square(int index) { return (board >> (index * 2)) & 3; };

  void set_square(int index, unsigned int nval) {
    unique_board_states[move_count()].insert(board);
    int binx = index * 2;
    board = (board | (3 << binx)) ^ (3 << binx) | (nval << binx);
  };

  int num_board_states(int move) { return unique_board_states[move].size(); };
  
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

  bool claim_win(int &block_square, unsigned int side) { return must_block(block_square, side, true); };
  
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
      // choose square for next move, this side...
      
      int ns;
      
      if (claim_win(ns,side)) {
	// this square wins...
	//std::cout << "WIN: ns: " << ns << " side: " << ((side==X) ? "X" : "O") << std::endl;
      } else if (must_block(ns,side)) {
	// block win for opponent...
	//std::cout << "BLOCK win for opponent: ns: " << ns << " side: " << ((side==X) ? "X" : "O") << std::endl;
      } else
	ns = (rand() & 0xf) % 9;

      // proceed only if square is free...
      if (square(ns) != FREE)
	continue;

      // make the move...
      //std::cout << " next move: ns " << ns << " side: " << ((side==X) ? "X" : "O") << std::endl;
      set_square(ns, side);
      record_move(ns, side);

      // check for win or draw...
      if (win(side)) {
	game_over = true;
	if (side==X)
	  X_wins = true;
	else
	  O_wins = true;
	if (display_outcome) std::cout << "WIN FOR " << (side==X ? "X" : "O") << "\n";
	break;
      }

      if (draw()) {
	game_over = true;
	its_a_draw = true;
	if (display_outcome) std::cout << "DRAW\n";
	break;
      }

      // switch sides...
      if (side == X)
	side = O;
      else
	side = X;
      
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
  std::set<unsigned int> unique_board_states[16];
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
  
  for (int i = 0; i < 10000; i++) {
    my_generator.random_game();
    my_generator.record_game();
  }

  my_generator.write_games_file("ttt_games_data_raw.xml");
  
  std::cout << "# of unique games:    " << my_generator.num_unique_games() << "\n";
  std::cout << "# of duplicate games: " << my_generator.num_duplicate_games() << "\n";
  std::cout << "# of wins for X:      " << my_generator.num_wins_X() << "\n";
  std::cout << "# of wins for O:      " << my_generator.num_wins_O() << "\n";
  std::cout << "# of draws:           " << my_generator.num_draws() << std::endl;
  std::cout << "shortest game:        " << my_generator.the_shortest_game() << std::endl;
  std::cout << "longest game:         " << my_generator.the_longest_game() << std::endl;
  int total_board_states = 0;
  for (int i = 0; (i < 16) && (my_generator.num_board_states(i) > 0); i++) {
    std::cout << "# of board states (move " << (i + 1) << "): " << my_generator.num_board_states(i) << std::endl;
    total_board_states += my_generator.num_board_states(i);
  }
  std::cout << "total # of board states: " << total_board_states << std::endl;

  std::cout << "\ngames data file: ttt_games_data.xml" << std::endl;
  
  return system("xmllint --format ttt_games_data_raw.xml >ttt_games_data.xml");
}

