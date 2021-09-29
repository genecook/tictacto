#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <exception>
#include <iostream>

#include <stdlib.h>
#include <time.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

namespace pt = boost::property_tree;

const pt::ptree& empty_ptree() {
  static pt::ptree t;
  return t;
}

class Qneuron {
public:
  Qneuron() {};
  Qneuron(unsigned long long _state, unsigned long long _input, bool _input_layer, bool _output_layer)
    : state(_state), input_layer(_input_layer), output_layer(_output_layer) {
    AddConnection(input_biases[_input]);
    srand48(time(NULL));
  };

  bool InputLayer() { return input_layer; };
  bool OutputLayer() { return output_layer; };
  unsigned long long NextState() { return state; }; // this state's value will be used as a 'connection' to any next state(s)
  void AddConnection(unsigned long long _input) {
    input_biases[_input] = drand48(); // bias between 0 and 1.0
  };
  float Bias(unsigned long long input) { return input_biases[input]; };
  void SetBias(unsigned long long input, float nval) { input_biases[input] = nval; };
  
private:
  unsigned long long state;
  bool input_layer;
  bool output_layer;
  std::map<unsigned long long, float> input_biases; // biases for each input
};

class QneuronNetwork {
public:
  QneuronNetwork() {};
  void AddNeuron(unsigned long long state, unsigned long long previous_state, bool input_layer=false, bool output_layer=false) {
    if (neurons.find(state) != neurons.end()) {
      // add connection to existing neuron...
      neurons[state].AddConnection(previous_state);
    } else {
      neurons[state] = Qneuron(state,previous_state,input_layer,output_layer);
    }

    std::map<unsigned long long, std::vector<unsigned long long>>::iterator ci = connections.find(previous_state);

    if (ci == connections.end()) {
      connections[previous_state].push_back(state);
      return;
    }
    
    bool connection_already_there = false;
    for (auto si = (ci->second).begin(); si != (ci->second).end(); si++) {
       if ( (*si) == state) {
	 connection_already_there = true;
	 break;
       }
    }
    if (!connection_already_there)
      connections[previous_state].push_back(state);
  };

  void ShowConnections() {
    std::cout << " # of neurons: " << neurons.size() << ", # of connections: " << connections.size() << std::endl;
    
    std::cout << "Neurons:";
    for (auto ni = neurons.begin(); ni != neurons.end(); ni++) {
      std::cout << " 0x" << std::hex << ni->second.NextState() << std::dec;
      std::cout << (ni->second.InputLayer() ? "(input)" : "") << (ni->second.OutputLayer() ? "(output)" : "");
    }
    std::cout << "\n";
    std::cout <<   "Connections:\n";
    for (auto ci = connections.begin(); ci != connections.end(); ci++) {
      std::cout << "             0x" << std::hex << ci->first << " --->" << std::dec;
       for (auto si = (ci->second).begin(); si != (ci->second).end(); si++) {
	 std::cout << " 0x" << std::hex << (*si) << std::dec;
       }
       std::cout << "\n";
    }
    std::cout << std::endl;
  };
  
private:
  std::map<unsigned long long, Qneuron> neurons; // set of all neurons, indexed by state
  std::map<unsigned long long, std::vector<unsigned long long>> connections; // connections between neurons...
};

enum SQUARE { O=1, X=2, FREE=0 };

class tictacto_neural_network_generator {
public:
  tictacto_neural_network_generator() {};
  tictacto_neural_network_generator(std::string neural_nets_builder_file,std::string games_file) {
    read_games_files(games_file);
    build_network();
    write_nets_builder_file(neural_nets_builder_file);
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

    unsigned long long previous_board_state = 0, next_board_state = 0;
    
    for (unsigned int i = 0; i < moves.size(); i++) {
       std::cout << "   move:  board index=" << moves[i].board_index << ", side=" << ((moves[i].side==X) ? "X" : "O");

       previous_board_state = next_board_state;

       int binx = moves[i].board_index * 2;
       next_board_state = (next_board_state | (3 << binx)) ^ (3 << binx) | (moves[i].side << binx);

       std::cout << " prev-state: 0x" << std::hex << previous_board_state << " next-state: 0x"
		 << next_board_state << std::dec << std::endl;
       
       my_network.AddNeuron( next_board_state, previous_board_state, (i==0), (i==(moves.size()-1) ) );
    }

    my_network.ShowConnections();
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

  void build_network() {
  };

  void write_nets_builder_file(std::string &neural_nets_builder_file) {
  };
  
private:
  QneuronNetwork my_network;
};

int main(int argc, char **argv) {
  std::cout << "Generating neural network structure from games database..." << std::endl;

  int rcode = 0;
  
  try {
    tictacto_neural_network_generator my_nets_builder("ttt_neural_nets_builder.xml", "ttt_games_data.xml");
    std::cout << "Neural nets builder file: " << "ttt_neural_nets_builder.xml" << std::endl;
  } catch(...) {
    rcode = -1;
    std::cerr << "Problems generating neural network?" << std::endl;
  }

  return rcode;
}
