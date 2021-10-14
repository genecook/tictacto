#include <tictacto_games_generator.h>

#include <algorithm>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

namespace pt = boost::property_tree;

//*****************************************************************************************
// after generating N games, write to (XML) file..
//*****************************************************************************************

void tictacto_games_generator::write_games_file(std::string gfile) {
    pt::ptree tree;

    int game_index = 0;

    for (auto uiter = unique_games.begin(); uiter != unique_games.end(); uiter++) {
       game_index++;

       pt::ptree game_subtree;
       game_subtree.put("<xmlattr>.number",game_index);
       
       char encoding[128];
       sprintf(encoding,"0x%08llx",uiter->first);
       game_subtree.put("<xmlattr>.moves_encoded",encoding);

       game_subtree.add("<xmlattr>.side",(uiter->second.side == X) ? "X" : "O");

       game_subtree.add("<xmlattr>.outcome",(uiter->second.is_win) ? "WIN" : "DRAW");
	 
       int index = 99;
       int side = FREE;

       pt::ptree moves_subtree;

       int mcnt = 0;
       for (int mi = 9; mi >= 0; mi--) {
	 unsigned int next_move = ((uiter->second.moves) >> (6 * mi)) & 0x3f;
	 if (next_move == 0)
	   continue;
	 mcnt++;
	 int index = next_move >> 2;
	 std::string side = ((next_move & 3) == 2) ? "X" : "O";

	 pt::ptree move_subtree;
       
	 move_subtree.add("<xmlattr>.number",mcnt);
	 move_subtree.add("<xmlattr>.index",index);
	 move_subtree.add("<xmlattr>.side",side);

         moves_subtree.add_child("move",move_subtree);	 
       }

       game_subtree.add_child("moves",moves_subtree);

       tree.add_child("games.game",game_subtree);
    }

    pt::write_xml(gfile,tree);
}

//*****************************************************************************************
// some simple-minded tic-tac-to heuristics...
//*****************************************************************************************

bool tictacto_games_generator::win(unsigned int side) {
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
}

bool tictacto_games_generator::must_block(int &block_square, unsigned int side, bool or_win) {
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
}

//*****************************************************************************************
// generate random games; retain games that end in win or draw...
//*****************************************************************************************

int tictacto_games_generator::random_square() {
  int free_square = -1;
  
  std::vector<int> free_squares;
  for (int i = 0; i < 9; i++) {
    if ( square(i) == FREE ) free_squares.push_back(i);
  }
  if (free_squares.size() > 0) {
    std::random_shuffle(free_squares.begin(),free_squares.end());
    free_square = free_squares[0];
  }
    
  return free_square;
};

unsigned int tictacto_games_generator::random_game(bool display_outcome) {
  std::cout << "random game..." << std::endl;

  display_outcome = true;
  // put come code here dude...

  init_for_next_game();

  bool game_over = false;

  int move_count = 0;
  
  side = X; // X always goes first...

  while(!game_over) {
    // make move, this side...
      
    int ns = random_square();

    if ( ns >= 0) {
      // make the move...
      std::cout << " next move: ns " << ns << " side: " << ((side==X) ? "X" : "O") << std::endl;
      set_square(ns, side);
      record_move(ns, side);
      move_count++;
    } else {
      // no free squares...
      game_over = true;
      its_a_draw = true;
      if (display_outcome) std::cout << "DRAW\n";
      side = ( (rand() & 0x1) == 1 ) ? X : O; // for a draw, just arbitrarily pick a side(?)
      break;
    }
    
    // check for win...
    if (win(side)) {
      game_over = true;
      if (side==X)
	X_wins = true;
      else {
	side = O;   // X went first, but O won!
	O_wins = true;
      }
      if (display_outcome) std::cout << "WIN FOR " << (side==X ? "X" : "O") << "\n";
      break;
    }

    // switch sides...
    side = (side == X) ? O : X;
  }

  
  std::cout << "random game ends after " << move_count << " moves." << std::endl;

  return moves;
}

//*****************************************************************************************
// computer always plays as X; use simple heuristics to increase chances of win; retain
// games that end in win or draw...
//*****************************************************************************************

unsigned int tictacto_games_generator::play_to_win(bool display_outcome) {
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
}


