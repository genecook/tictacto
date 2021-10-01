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

class Qstate {
public:
  Qstate() {};
  Qstate(unsigned int _state, unsigned int _action) : state(_state) {
    AddAction(_action);
  };

  unsigned int State() { return state; };

  void AddAction(unsigned int _action) {
    if (actions.find(_action) == actions.end())
      actions[(_action)] = 0.0;
  };
  
  bool ActionBias(float &bias, unsigned int _action) {
    std::map<unsigned int, float>::iterator a_iter = actions.find(_action);
    if (a_iter == actions.end())
      return false;
    bias = actions[_action];
    return true;
  };
  
  void UpdateActionBias(unsigned int _action, float _qval) { actions[_action] = _qval; };

  void Actions(std::vector<unsigned int> &_actions) {
    for (auto a_iter = actions.begin(); a_iter != actions.end(); a_iter++) {
       _actions.push_back(a_iter->first);
    }
  };
  
private:
  unsigned int state;
  std::map<unsigned int, float> actions; // index is action, value is Q-value for each action
};


class Qtable {
public:
  Qtable() {};

  void AddState(unsigned int _state, int _action) {
    std::map<unsigned int, Qstate>::iterator s_iter = states.find(_state);
    if (s_iter != states.end())
      throw std::runtime_error("State already exists???");
    states[_state] = Qstate(_state,_action);
  };

  bool StateExists(unsigned int _state) { return (states.find(_state) != states.end()); };
  
  void AddAction(unsigned int _state, int _action) {
    std::map<unsigned int, Qstate>::iterator s_iter = states.find(_state);
    if (s_iter == states.end())
      throw std::runtime_error("State does not exist???");
    s_iter->second.AddAction(_action);
  };
  
  void GetActions(std::vector<unsigned int> &_actions, unsigned int _state) {
    std::map<unsigned int, Qstate>::iterator s_iter = states.find(_state);
    if (s_iter == states.end())
      throw std::runtime_error("State does not exist???");
    s_iter->second.Actions(_actions);
  };

  bool GetActionBias(float &_bias, unsigned int _state, unsigned int _action) {
    std::map<unsigned int, Qstate>::iterator s_iter = states.find(_state);
    if (s_iter == states.end())
      throw std::runtime_error("State does not exist???");
    return s_iter->second.ActionBias(_bias,_action);
  };
  
private:
  std::map<unsigned int, Qstate> states; // set of all states
};

