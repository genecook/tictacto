#include <qneuron.h>

void QneuronNetwork::AddState(unsigned long long state, unsigned long long previous_state, int action) {
    if (states.find(state) != states.end()) {
      // add connection to existing neuron...
      states[state].InitActionBias(previous_state);
    } else {
      states[state] = Qstate(state,action);
    }

    std::map<unsigned long long, std::vector<unsigned long long>>::iterator ci = actions.find(previous_state);

    if (ci == actions.end()) {
      action[previous_state].push_back(state);
      return;
    }
    
    bool action_already_there = false;
    for (auto si = (ci->second).begin(); si != (ci->second).end(); si++) {
       if ( (*si) == state) {
	 action_already_there = true;
	 break;
       }
    }
    if (!action_already_there)
      actions[previous_state].push_back(state);
}

void QneuronNetwork::ShowConnections() {
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
}

bool Qneuron::GetConnection(float &bias, unsigned long long state) {
}

// given an ordered set of states (neuron 'indices'), traverse the network and confirm the connections

bool QneuronNetwork::Validate(std::vector<unsigned long long> expected_states) {
  if (expected_states.size() < 2)
    return false;

  // 1st neuron in expected states should be on 'input' layer...
  
  std::map<unsigned long long, Qneuron>::iterator input_layer_neuron = neurons.find(expected_states[0]);
  
  if (input_layer_neuron == neurons.end())
    return false;

  if (!input_layer_neuron->second.InputLayer())
    return false;
  
  for (int esi = 0; esi < expected_states.size(); esi++) {
     std::map<unsigned long long, std::vector<unsigned long long>>::iterator neuron_connections = connections.find(expected_states[esi]);
  }
  

    
}
