#include <tictacto_qtable_generator.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

namespace pt = boost::property_tree;

const pt::ptree& qtable_generator_empty_ptree() {
  static pt::ptree t;
  return t;
}

void tictacto_qtable_generator::extract_states(int game_number, unsigned long long encoded_moves, int outcome,
					       int computers_side, std::vector<struct move> &moves) {
    std::cout << "game " << game_number << std::endl;
    std::cout << "   encoded moves: 0x" << std::hex << encoded_moves << std::dec << std::endl;
    switch(outcome) {
      case WIN:  std::cout << "   outcome: WIN" << std::endl;
      case LOSS: std::cout << "   outcome: LOSS" << std::endl;
      case DRAW: std::cout << "   outcome: DRAW" << std::endl;
      default: break;
    }
    std::cout << "   side: " << ((computers_side == X) ? "X" : "O") << std::endl;
    
    unsigned int previous_board_state = 0, next_board_state = 0;

    struct move_state_q {
      move_state_q(unsigned int _board_state, unsigned int _action)
	: board_state(_board_state), action(_action) {};
      unsigned int board_state;
      unsigned int action;
    };

    std::vector<struct move_state_q> moves_for_bias_update;
    
    for (unsigned int i = 0; i < moves.size(); i++) {
      std::cout << "   move:  board index=" << moves[i].board_index
		<< ", side=(" << moves[i].side << ")"
		<< ((moves[i].side==X) ? "X" : "O");

       previous_board_state = next_board_state;

       bool is_computers_move = (moves[i].side==computers_side);
       
       unsigned int binx = moves[i].board_index << 2;
       next_board_state = next_board_state | (moves[i].side << binx);

       unsigned int action = binx | moves[i].side;
       
       std::cout << " prev-state: 0x" << std::hex << previous_board_state
		 << " next-state: 0x" << next_board_state << std::dec << std::endl;

       try {
         my_qtable.AddState( previous_board_state, action );
	 if (is_computers_move) {
	   struct move_state_q X(previous_board_state,action);
	   moves_for_bias_update.push_back(X);
	 }
       } catch(std::runtime_error & e) {
	 std::cout << e.what() << std::endl;
	 throw std::runtime_error("One or more Qtable errors. Cannot continue.");
       }
    }

    // update 'winning' move action-Q values for 'this' side, in reverse order, as each
    // previous move's Q value is proportional to the next move Q value...
    
    std::reverse(moves_for_bias_update.begin(), moves_for_bias_update.end());

    float incrementalQ; 

    switch(outcome) {
      case WIN:  incrementalQ = 0.9;  break; // larger reward and thus Q for win vs draw
      case DRAW: incrementalQ = 0.6;  break;
      case LOSS: incrementalQ = -0.9; break; // penalty for loss
      default: break;
    }

    for (auto m_iter = moves_for_bias_update.begin(); m_iter != moves_for_bias_update.end(); m_iter++) {
      std::cout << "!!! updating bias for state/action 0x" << std::hex << (*m_iter).board_state
		<< "/0x" << (*m_iter).action << std::dec << std::endl;
      my_qtable.UpdateActionBias( (*m_iter).board_state, (*m_iter).action, incrementalQ );
      std::cout << "!!! updated bias: " << my_qtable.GetActionBias((*m_iter).board_state, (*m_iter).action)
		<< std::endl;
      // update incrementalQ, ask me how...
      incrementalQ *= 0.9;
    }
}

void tictacto_qtable_generator::read_games_files(std::string &games_file) {
    pt::ptree tree;
    pt::read_xml(games_file, tree);

    BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("games")) {
      pt::ptree game_subtree = v.second;

      std::string at = v.first + ".<xmlattr>";
      
      const pt::ptree & attributes = v.second.get_child("<xmlattr>", qtable_generator_empty_ptree());

      int game_number = -1;
      unsigned long long encoded_moves = 0;
      int outcome = -1;
      int winning_side = -1;
      
      BOOST_FOREACH(const pt::ptree::value_type &va, attributes) {
	if (!strcmp(va.first.data(),"number"))
	  sscanf(va.second.data().c_str(),"%d",&game_number);
	else if (!strcmp(va.first.data(),"moves_encoded"))
	  sscanf(va.second.data().c_str(),"0x%llx",&encoded_moves);
	else if (!strcmp(va.first.data(),"side"))
	  winning_side = !strcmp(va.second.data().c_str(),"X") ? X : O;
	else if (!strcmp(va.first.data(),"outcome")) {
	  if (va.second.data() == "WIN")
	    outcome = WIN;
	  else if (va.second.data() == "LOSS")
	    outcome = LOSS;
	  else
	    outcome = DRAW;
	  }
      }
      
      std::vector<struct move> moves;
      
      BOOST_FOREACH(pt::ptree::value_type &mv, game_subtree.get_child("moves")) {
        std::string at = mv.first + ".<xmlattr>";
      
        const pt::ptree & attributes = mv.second.get_child("<xmlattr>", qtable_generator_empty_ptree());

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

      extract_states(game_number,encoded_moves,outcome,winning_side,moves);
    }
}

