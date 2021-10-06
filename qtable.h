#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <exception>
#include <iostream>

#include <stdlib.h>
#include <time.h>

//    srand48(time(NULL));

//*************************************************************************************************
// A 'Qstate' has a state variable, and a set of biased actions.  
//*************************************************************************************************

class Qstate {
public:
  Qstate() {};

  // a Qstate has a defined state value, and at least one action possible...
  
  Qstate(unsigned int _state, unsigned int _action) : state(_state) {
    AddAction(_action);
  };

  unsigned int State() { return state; };

  // by 'adding' an action, we're really specifying a bias (Q value) for an action
  // that is possible from this state...
  
  void AddAction(unsigned int _action) {
    if (actions.find(_action) != actions.end()) {
      // a Q value for this action has already been specified. don't overwrite it...
    } else {
      actions[(_action)] = 0.0; // default Q value is zero
    }
  };

  // return bias (Q value) for an action, or false should this action not be found,
  // is, is not possible, from this state...
  
  float ActionBias(unsigned int _action) {
    std::map<unsigned int, float>::iterator a_iter = actions.find(_action);
    if (a_iter == actions.end())
      throw std::runtime_error("Action does not exist???");
    return actions[_action];
  };

  // update the q value for some action...
  
  void UpdateActionBias(unsigned int _action, float _qval) {
    actions[_action] += _qval;
    std::cout << "[UpdateActionBias] state: 0x" << std::hex << State()
	      << " action: 0x" << _action
	      << " bias: " << std::dec << actions[_action] << std::endl;
  };

  // return the set of actions possible from this state...
  
  void Actions(std::vector<unsigned int> &_actions) {
    for (auto a_iter = actions.begin(); a_iter != actions.end(); a_iter++) {
       _actions.push_back(a_iter->first);
    }
  };

  int ActionCount() { return actions.size(); };
  
  // return action with highest 'Q' value...
  
  unsigned int ChooseAction() {
    float high_qval = -1.0;
    unsigned int action_chosen = 0;
    
    for (auto a_iter = actions.begin(); a_iter != actions.end(); a_iter++) {
      if (a_iter->second > high_qval) {
        high_qval = a_iter->second;
        action_chosen = a_iter->first;
      }
    }

    if (high_qval == -1.0)
      throw std::runtime_error("No 'biased' action???");
      
    return action_chosen;
  };

private:
  unsigned int state;
  std::map<unsigned int, float> actions; // index is action, value is Q-value for each action
};


//*************************************************************************************************
// A 'Qtable' is a set of Qstates, indexed by some (next) state...
//*************************************************************************************************

class Qtable {
public:
  Qtable() {};

  void AddState(unsigned int _state, int _action) {
    unsigned int inx = _action >> 2;
    unsigned int side = _action & 3;
    std::cout << "      [AddState] state: 0x" << std::hex << _state << " action: 0x" << _action
	      << "( index: 0x" << inx << ",side: " << side << ")" << std::dec << std::endl;
    std::map<unsigned int, Qstate>::iterator s_iter = states.find(_state);
    if (s_iter == states.end())
      states[_state] = Qstate(_state,_action);
    else
      s_iter->second.AddAction(_action);
  };

  bool StateExists(unsigned int _state) { return (states.find(_state) != states.end()); };
  
  void AddAction(unsigned int _state, int _action) {
    std::map<unsigned int, Qstate>::iterator s_iter = states.find(_state);
    if (s_iter == states.end())
      throw std::runtime_error("State does not exist???");
    s_iter->second.AddAction(_action);
  };

  // update the q value for some action...
  
  void UpdateActionBias(unsigned int _state, unsigned int _action, float _qval) {
    std::map<unsigned int, Qstate>::iterator s_iter = states.find(_state);
    if (s_iter == states.end())
      throw std::runtime_error("State does not exist???");
    s_iter->second.UpdateActionBias(_action,_qval);
  };

  void GetActions(std::vector<unsigned int> &_actions, unsigned int _state) {
    std::map<unsigned int, Qstate>::iterator s_iter = states.find(_state);
    if (s_iter == states.end())
      throw std::runtime_error("State does not exist???");
    s_iter->second.Actions(_actions);
  };

  float GetActionBias(unsigned int _state, unsigned int _action) {
    std::map<unsigned int, Qstate>::iterator s_iter = states.find(_state);
    if (s_iter == states.end())
      throw std::runtime_error("State does not exist???");
    return s_iter->second.ActionBias(_action);
  };

  int StatesCount() { return states.size(); };

  int HighActionCount() {
    int high_count = 0;
    for (auto s_iter = states.begin(); s_iter != states.end(); s_iter++) {
      if (s_iter->second.ActionCount() > high_count)
	high_count = s_iter->second.ActionCount();
    }
    return high_count;
  };

  void ReadQtableFile(std::string &qtable_file);
  void WriteQtableFile(std::string &qtable_file);
  
private:
  std::map<unsigned int, Qstate> states; // set of all states
};

