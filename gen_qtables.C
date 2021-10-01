#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <exception>
#include <iostream>

#include <stdlib.h>
#include <time.h>

#include <qtable.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

namespace pt = boost::property_tree;

const pt::ptree& empty_ptree() {
  static pt::ptree t;
  return t;
}

enum SQUARE { O=1, X=2, FREE=0 };

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

  void extract_states(int game_number, unsigned long long encoded_moves, bool is_win, std::vector<struct move> &moves) {
    std::cout << "game " << game_number << std::endl;
    std::cout << "   encoded moves: 0x" << std::hex << encoded_moves << std::dec << std::endl;
    std::cout << "   outcome: " << (is_win ? "WIN" : "DRAW") << std::endl;

    unsigned int previous_board_state = 0, next_board_state = 0;
    
    for (unsigned int i = 0; i < moves.size(); i++) {
       std::cout << "   move:  board index=" << moves[i].board_index << ", side=" << ((moves[i].side==X) ? "X" : "O");

       previous_board_state = next_board_state;

       unsigned int binx = moves[i].board_index * 2;
       next_board_state = (next_board_state | (3 << binx)) ^ (3 << binx) | (moves[i].side << binx);

       unsigned int action = binx | moves[i].side==X;
       
       std::cout << " prev-state: 0x" << std::hex << previous_board_state << " next-state: 0x"
		 << next_board_state << std::dec << std::endl;

       try {
         my_qtable.AddState( previous_board_state, action );
       } catch(std::runtime_error & e) {
	 std::cout << e.what() << std::endl;
	 throw std::runtime_error("One or more Qtable errors. Cannot continue.");
       }
    }
  };
  
  void read_games_files(std::string &games_file) {
    pt::ptree tree;
    pt::read_xml(games_file, tree);

    BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("games")) {
      pt::ptree game_subtree = v.second;

      std::string at = v.first + ".<xmlattr>";
      
      const pt::ptree & attributes = v.second.get_child("<xmlattr>", empty_ptree());

      int game_number = -1;
      unsigned long long encoded_moves = 0;
      bool is_win = false;
      
      BOOST_FOREACH(const pt::ptree::value_type &va, attributes) {
	if (!strcmp(va.first.data(),"number"))
	  sscanf(va.second.data().c_str(),"%d",&game_number);
	else if (!strcmp(va.first.data(),"moves_encoded"))
	  sscanf(va.second.data().c_str(),"0x%llx",&encoded_moves);
	else
	  is_win = !strcmp(va.first.data(),"outcome") && (va.second.data() == "WIN");
      }
      
      std::vector<struct move> moves;
      
      BOOST_FOREACH(pt::ptree::value_type &mv, game_subtree.get_child("moves")) {
        std::string at = mv.first + ".<xmlattr>";
      
        const pt::ptree & attributes = mv.second.get_child("<xmlattr>", empty_ptree());

	int move_number = -1;
	int board_index = -1;
	int side = O;
	  
	BOOST_FOREACH(const pt::ptree::value_type &va, attributes) {	
	  if (!strcmp(va.first.data(),"number"))
	    sscanf(va.second.data().c_str(),"%d",&move_number);
	  else if (!strcmp(va.first.data(),"index"))
	    sscanf(va.second.data().c_str(),"%d",&board_index);
	  else if (!strcmp(va.first.data(),"side"))
	    side = (va.second.data() == "X") ? X : O;
       	}
	
        moves.push_back(move(board_index,side));
      }

      extract_states(game_number,encoded_moves,is_win,moves);
    }

  };

  void write_qtable_file(std::string &qtable_file) {
  };
  
private:
  Qtable my_qtable;
};

int main(int argc, char **argv) {
  std::cout << "Generating neural network structure from games database..." << std::endl;

  int rcode = 0;
  
  try {
    tictacto_qtable_generator my_qtable_builder("ttt_qtable_data.xml", "ttt_games_data.xml");
    std::cout << "Qtable data file: " << "ttt_qtable_data.xml" << std::endl;
  } catch(...) {
    rcode = -1;
    std::cerr << "Problems generating qtable from games data?" << std::endl;
  }

  return rcode;
}
