#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <exception>
#include <iostream>

#include <time.h>

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

  void write_games_file(std::string gfile);

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
    if (unique_board_states[move_count()].find(board) != unique_board_states[move_count()].end()) {
      //std::cout << "Board state/move 0x" << std::hex << board << std::dec << "/" << move_count() << " is NOT unique?" << std::endl;
    } else
      unique_board_states[move_count()].insert(board);
    int binx = index * 2;
    board = (board | (3 << binx)) ^ (3 << binx) | (nval << binx);
  };

  int num_board_states(int move) { return unique_board_states[move].size(); };

  bool win(unsigned int side);
  bool must_block(int &block_square, unsigned int side, bool or_win = false);

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

  unsigned int random_game(bool display_outcome = false);

  void gen_random_game_set(int number_of_games) {
    for (int i = 0; i < number_of_games; i++) {
       random_game();
       record_game();
    }
  };

  void dump_game_stats() {
    std::cout << "# of unique games:    " << num_unique_games() << "\n";
    std::cout << "# of duplicate games: " << num_duplicate_games() << "\n";
    std::cout << "# of wins for X:      " << num_wins_X() << "\n";
    std::cout << "# of wins for O:      " << num_wins_O() << "\n";
    std::cout << "# of draws:           " << num_draws() << std::endl;
    std::cout << "shortest game:        " << the_shortest_game() << std::endl;
    std::cout << "longest game:         " << the_longest_game() << std::endl;

    int total_board_states = 0;
  
    for (int i = 0; (i < 16) && (num_board_states(i) > 0); i++) {
      std::cout << "# of board states (move " << (i + 1) << "): "
		<< num_board_states(i) << std::endl;
      total_board_states += num_board_states(i);
    }
  
    std::cout << "total # of board states: " << total_board_states << std::endl;
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


  
