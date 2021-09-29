#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <exception>
#include <iostream>

#include <stdlib.h>
#include <time.h>

class Qneuron {
public:
  Qneuron() {};
  Qneuron(unsigned long long _state, unsigned long long _input, int _action, bool _input_layer, bool _output_layer)
    : state(_state), input_layer(_input_layer), output_layer(_output_layer), action(_action) {
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
  int Action() { return action; };
  
private:
  unsigned long long state;
  bool input_layer;
  bool output_layer;
  std::map<unsigned long long, float> input_biases; // biases for each input
  int action;
};

class QneuronNetwork {
public:
  QneuronNetwork() {};
  void AddNeuron(unsigned long long state, unsigned long long previous_state, int action = -1, bool input_layer=false, bool output_layer=false);
  void ShowConnections();
  
private:
  std::map<unsigned long long, Qneuron> neurons; // set of all neurons, indexed by state
  std::map<unsigned long long, std::vector<unsigned long long>> connections; // connections between neurons...
};

