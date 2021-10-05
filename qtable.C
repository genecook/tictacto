#include <qtable.h>

#include <time.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

namespace pt = boost::property_tree;


void Qtable::ReadQtableFile(std::string &qtable_file) {
}

void Qtable::WriteQtableFile(std::string &qtable_file) {
    pt::ptree tree;

    for (auto s_iter = states.begin(); s_iter != states.end(); s_iter++) {
       pt::ptree qstate_subtree;
       
       char board_state[128];
       sprintf(board_state,"0x%08x",s_iter->first);
       qstate_subtree.put("<xmlattr>.state",board_state);

       pt::ptree actions_subtree;

       std::vector<unsigned int> actions;
       s_iter->second.Actions(actions);
	       
       for (auto a_iter = actions.begin(); a_iter != actions.end(); a_iter++) {
          pt::ptree action_subtree;
	  char action_str[128];
	  sprintf(action_str,"0x%x",*a_iter);
	  action_subtree.put("<xmlattr>.action",action_str);
	  int move_index = *a_iter >> 2;
	  std::string move_side = ((*a_iter & 3) == 2) ? "X" : "O";
	  char move_qbias[128];
	  sprintf(move_qbias,"%f",GetActionBias(s_iter->first,*a_iter));
	  action_subtree.put("<xmlattr>.board_index",move_index);
	  action_subtree.put("<xmlattr>.side",move_side);
	  action_subtree.put("<xmlattr>.qbias",move_qbias);
	  
	  actions_subtree.add_child("action",action_subtree);
       }

       qstate_subtree.add_child("actions",actions_subtree);
       
       tree.add_child("qstates.qstate",qstate_subtree);
    }

    pt::write_xml(qtable_file,tree);
}

