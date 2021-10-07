#include <iostream>
#include <stdlib.h>

#include <tictacto_qtable_generator.h>

int main(int argc, char **argv) {
  std::cout << "Generating neural network structure from games database..." << std::endl;
  int rcode = 0;
  
  try {
    tictacto_qtable_generator my_qtable_builder("ttt_qtable_data_raw.xml", "ttt_games_data.xml");
    rcode = system("xmllint --format ttt_qtable_data_raw.xml >ttt_qtable_data.xml");
    std::cout << "Qtable data file: " << "ttt_qtable_data.xml" << std::endl;
  } catch(...) {
    rcode = -1;
    std::cerr << "Problems generating qtable from games data?" << std::endl;
  }

  return rcode;
}
