#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <exception>
#include <iostream>
#include <algorithm>

#include <stdlib.h>
#include <time.h>

#include <qtable.h>

enum SQUARE { O=1, X=2, FREE=0 };

int main(int argc, char **argv) {
  std::cout << "Dude!" << std::endl;

  Qtable my_qtable;

  std::string qtable_file = "ttt_qtable_data.xml";
  
  my_qtable.ReadQtableFile(qtable_file);
  
  return 0;
}
