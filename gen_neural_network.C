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


class tictacto_neural_network_generator {
public:
  tictacto_neural_network_generator() {};
  tictacto_neural_network_generator(std::string neural_nets_builder_file,std::string games_file) {
    read_games_files(games_file);
    build_network();
    write_nets_builder_file(neural_nets_builder_file);
  };

  void read_games_files(std::string &games_file) {
    pt::ptree tree;
    pt::read_xml(games_file, tree);

    BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("games")) {
      std::cout << v.first.data() << std::endl;
      pt::ptree game_subtree = v.second;
      std::string encoded_moves = game_subtree.get<std::string>("encoded_moves");
      std::cout << "   " << encoded_moves << std::endl;
      BOOST_FOREACH(pt::ptree::value_type &m, game_subtree.get_child("moves")) {
	std::cout << "   " << m.first.data() << std::endl;
      }
    }

  };

  void build_network() {
  };

  void write_nets_builder_file(std::string &neural_nets_builder_file) {
  };
  
private:
  
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
