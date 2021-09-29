#include <qneuron.h>

void QneuronNetwork::AddNeuron(unsigned long long state, unsigned long long previous_state, int action, bool input_layer, bool output_layer) {
    if (neurons.find(state) != neurons.end()) {
      // add connection to existing neuron...
      neurons[state].AddConnection(previous_state);
    } else {
      neurons[state] = Qneuron(state,previous_state,action, input_layer,output_layer);
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
