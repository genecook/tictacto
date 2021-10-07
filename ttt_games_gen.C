#include <iostream>

#include <tictacto_games_generator.h>

int main(int argc, char **argv) {
  std::cout << "Generating random tic-tac-to games..." << std::endl;

  tictacto_games_generator my_generator;

  my_generator.gen_random_game_set(10000);

  my_generator.write_games_file("ttt_games_data_raw.xml");

  my_generator.dump_game_stats();

  std::cout << "\ngames data file: ttt_games_data.xml" << std::endl;
  
  return system("xmllint --format ttt_games_data_raw.xml >ttt_games_data.xml");
}

