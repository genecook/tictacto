#include <iostream>

#include <tictacto_games_generator.h>

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
    std::cout << "# of board states (move " << (i + 1) << "): "
	      << my_generator.num_board_states(i) << std::endl;
    total_board_states += my_generator.num_board_states(i);
  }
  
  std::cout << "total # of board states: " << total_board_states << std::endl;

  std::cout << "\ngames data file: ttt_games_data.xml" << std::endl;
  
  return system("xmllint --format ttt_games_data_raw.xml >ttt_games_data.xml");
}

