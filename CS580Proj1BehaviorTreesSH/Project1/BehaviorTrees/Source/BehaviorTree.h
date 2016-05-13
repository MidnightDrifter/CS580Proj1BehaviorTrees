//	File name:		BehaviorTree.h
//	Author(s):		Allan Deutsch
//	
//	All content 2014-2015 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "global.h"

/* agent communication BEGIN (by ABLA, Abdel Rahman) */
#include "msg.h"
#include "gameobject.h"
#include "msgroute.h"
#include "statemch.h"
#include <set>
#include <unordered_map>
#define ONE_FRAME (0.0001f)
/* agent communication END */


#define DEBUG_BT 1       // Set this to 1 to turn on text for behavior trees
#define ShortTextOn 1    // set this to 1 to shorten the text to the bottom 2 nodes, or 0 for the full node list.

// Use these macros to modify the parameters that get passed through the behavior tree as it updates.
// If there is any information (such as a message) that you wish to have passed through tree when it is updated,
// you can use these macros to do that easily without touching the other code.
// A way to access the object instance, and the current frame timestep are highly recommended.
#define BT_UPDATE_FUNC_PARAMS objectID self, float dt
// This macro should have all of the argument names from the BT_UPDATE_FUNC_PARAMS macro, but without any typenames or decorators (*/&/const/volatile) included.
#define BT_UPDATE_FUNC_PARAMS_UNDECORATED self, dt


/*                               WARNING
					It is recommended that you do not modify any code below this line, 
					and instead do so using the macros and typedef above.
					Proceed at your own risk.
                                 .i;;;;i.                                  
                               iYcviii;vXY:                                
                             .YXi       .i1c.                              
                            .YC.     .    in7.                             
                           .vc.   ......   ;1c.                            
                           i7,   ..        .;1;                            
                          i7,   .. ...      .Y1i                           
                         ,7v     .6MMM@;     .YX,                          
                        .7;.   ..IMMMMMM1     :t7.                         
                       .;Y.     ;$MMMMMM9.     :tc.                        
                       vY.   .. .nMMM@MMU.      ;1v.                       
                      i7i   ...  .#MM@M@C. .....:71i                       
                     it:   ....   $MMM@9;.,i;;;i,;tti                      
                    :t7.  .....   0MMMWv.,iii:::,,;St.                     
                   .nC.   .....   IMMMQ..,::::::,.,czX.                    
                  .ct:   ....... .ZMMMI..,:::::::,,:76Y.                   
                  c2:   ......,i..Y$M@t..:::::::,,..inZY                   
                 vov   ......:ii..c$MBc..,,,,,,,,,,..iI9i                  
                i9Y   ......iii:..7@MA,..,,,,,,,,,....;AA:                 
               iIS.  ......:ii::..;@MI....,............;Ez.                
              .I9.  ......:i::::...8M1..................C0z.               
             .z9;  ......:i::::,.. .i:...................zWX.              
             vbv  ......,i::::,,.      ................. :AQY              
            c6Y.  .,...,::::,,..:t0@@QY. ................ :8bi             
           :6S. ..,,...,:::,,,..EMMMMMMI. ............... .;bZ,            
          :6o,  .,,,,..:::,,,..i#MMMMMM#v.................  YW2.           
         .n8i ..,,,,,,,::,,,,.. tMMMMM@C:.................. .1Wn           
         7Uc. .:::,,,,,::,,,,..   i1t;,..................... .UEi          
         7C...::::::::::::,,,,..        ....................  vSi.         
         ;1;...,,::::::,.........       ..................    Yz:          
          v97,.........                                     .voC.          
           izAotX7777777777777777777777777777777777777777Y7n92:            
             .;CoIIIIIUAA666666699999ZZZZZZZZZZZZZZZZZZZZ6ov.    
*/

using std::vector;
using std::string;
void PushText(objectID, string);

enum NodeStatus
{
  NS_Failed,
  NS_Completed,
  NS_Running,
  NS_OnEnter
};

enum NodeType
{
  NT_Selector = 0,
  NT_Decorator = 1,
  NT_Leaf = 2
};
struct IBTNode
{
  virtual ~IBTNode() {}
  IBTNode(string name, string description, NodeType type, string type_name) : m_name(name), m_description(description), 
    m_type(type), m_type_name(type_name), currentStatus(NS_OnEnter) {}
  string m_name;
  const string m_description;
  IBTNode *p_parent;
  virtual NodeStatus Update(BT_UPDATE_FUNC_PARAMS);
  virtual NodeStatus UpdateLogic(BT_UPDATE_FUNC_PARAMS) = 0;
  virtual void OnEdit(){};
  const NodeType m_type;
  std::string m_type_name;
  /* agent communication BEGIN (by ABLA, Abdel Rahman) */
  float time;
  float period;
  virtual void OnMessageReceived(MSG_Name name, objectID self, MSG_Data data) = 0;

  void SendMsg(MSG_Name name, objectID receiver, objectID self, std::string node_name, MSG_Data& data)
  {
	  //ASSERTMSG( receiver != m_owner->GetID(), "StateMachine::SendMsg - Do not use to send messages to your own state machine. Use the other variations, such as SendMsgToState or SendMsgToStateMachine." );
	  g_msgroute.SendMsg(ONE_FRAME, name, receiver, self, node_name, data, false, false);

  }

  void SendMsgDelayed(float delay, MSG_Name name, objectID receiver, objectID self, std::string node_name, MSG_Data& data)
  {
	  g_msgroute.SendMsg(delay, name, receiver, self, node_name, data, false, false);
  }

  virtual void ReceiveMessage(MSG_Name name, MSG_Data& data, string node_name, objectID self)
  {
	  if (node_name.empty() || (m_name == node_name))
	  {
		  OnMessageReceived(name, self, data);
	  }
  }
  /* agent communication END */

  void ResetStatus()
  { 
	  currentStatus = NS_OnEnter;
  }
  NodeStatus GetCurrentStatus() const
  {
    return currentStatus;
  }
  protected:
	  NodeStatus currentStatus;
};

struct IBTLogicNode : IBTNode
{
  virtual ~IBTLogicNode() { freeKids(); }
  IBTLogicNode(string name, string description, NodeType type, string type_name) : IBTNode(name, description, type, type_name), childStatus(NS_OnEnter), m_currentChildIndex(-1) {}
  vector<IBTNode *> m_children;
  NodeStatus Update(BT_UPDATE_FUNC_PARAMS);
  virtual NodeStatus UpdateLogic(BT_UPDATE_FUNC_PARAMS) = 0;
  int m_currentChildIndex;

  /* agent communication BEGIN (by ABLA, Abdel Rahman) */
  virtual void OnMessageReceived(MSG_Name name, objectID self, MSG_Data data) = 0;
  std::set<int> chosenChildren;

  void ReceiveMessage(MSG_Name name, MSG_Data& data, std::string node_name, objectID self)
  {
	  if (m_name != node_name)
	  {
		  if (node_name == "DecoratorInterrupt")
		  {
			  for (vector<IBTNode *>::iterator it = m_children.begin(); it != m_children.end(); ++it)
			  {
				  IBTLogicNode *logic = dynamic_cast<IBTLogicNode*>(*it);
				  if (logic)
					  logic->ReceiveMessage(name, data, node_name, self);
				  else
					  (*it)->ReceiveMessage(name, data, node_name, self);
			  }
		  }
		  else
		  {
			  if (m_currentChildIndex != -1)
				  m_children[m_currentChildIndex]->ReceiveMessage(name, data, node_name, self);
		  }

	  }
	  else
	  {
		  OnMessageReceived(name, self, data);
	  }
  }
  /* agent communication END */

  void ResetLogicStatus()
  {
	  ResetStatus();
	  m_currentChildIndex = -1;

	  for (vector<IBTNode *>::iterator it = m_children.begin(); it != m_children.end(); ++it)
	  {
		  IBTLogicNode *logic = dynamic_cast<IBTLogicNode*>(*it);

		  if (logic)
		  {
			  logic->ResetLogicStatus();
		  }
		  else
		  {
			  (*it)->ResetStatus();
		  }
	  }
  }

protected:
  void freeKids()
  {
    for (vector<IBTNode *>::iterator it = m_children.begin(); it != m_children.end(); ++it)
    {
      if (*it != nullptr)
      {
        delete *it;
        *it = 0;
      }
    }
  }

  NodeStatus childStatus;
};

typedef IBTNode * (*LeafNodeCreateFn)();
typedef IBTLogicNode * (*LogicNodeCreateFn)();
typedef std::pair<NodeType, LogicNodeCreateFn> LogicNodeInfo;

/* agent communication BEGIN (by ABLA, Abdel Rahman) */
/*
#define DEFINE_LEAF_NODE(NAME, DESCRIPTION, VARS) \
struct NAME : public IBTNode\
{ \
  static IBTNode *CreateInstance() { return new NAME; } \
  ~##NAME() { } \
  NAME() : IBTNode(#NAME, DESCRIPTION, NT_Leaf){} \
  NAME(std::string name) : IBTNode(name, DESCRIPTION, NT_Leaf){} \
  NodeStatus UpdateLogic(BT_UPDATE_FUNC_PARAMS); \
  VARS \
};
*/

#define DEFINE_LEAF_NODE(NAME, DESCRIPTION, VARS) \
struct NAME : public IBTNode\
{ \
  static IBTNode *CreateInstance() { return new NAME; } \
  ~##NAME() { } \
  NAME() : IBTNode(#NAME, DESCRIPTION, NT_Leaf,#NAME){} \
  NAME(std::string name) : IBTNode(name, DESCRIPTION, NT_Leaf,#NAME){} \
  NodeStatus UpdateLogic(BT_UPDATE_FUNC_PARAMS); \
  void OnMessageReceived(MSG_Name name, objectID self, MSG_Data data);\
  VARS \
};\
/* agent communication END */

/* agent communication BEGIN (by ABLA, Abdel Rahman) */
/*
#define DEFINE_SELECTOR_NODE(NAME, DESCRIPTION, VARS) \
struct NAME : public IBTLogicNode\
{ \
  static IBTLogicNode *CreateInstance() { return new NAME; } \
  ~##NAME()  { freeKids(); } \
  NAME() : IBTLogicNode(#NAME, DESCRIPTION, NT_Selector){} \
  NAME(std::string name) : IBTLogicNode(name, DESCRIPTION, NT_Selector){} \
  NodeStatus UpdateLogic(BT_UPDATE_FUNC_PARAMS); \
  VARS \
};
*/

#define DEFINE_SELECTOR_NODE(NAME, DESCRIPTION, VARS) \
struct NAME : public IBTLogicNode\
{ \
  static IBTLogicNode *CreateInstance() { return new NAME; } \
  ~##NAME()  { freeKids(); } \
  NAME() : IBTLogicNode(#NAME, DESCRIPTION, NT_Selector,#NAME){} \
  NAME(std::string name) : IBTLogicNode(name, DESCRIPTION, NT_Selector,#NAME){} \
  NodeStatus UpdateLogic(BT_UPDATE_FUNC_PARAMS); \
  void OnMessageReceived(MSG_Name name, objectID self, MSG_Data data);\
  VARS \
};
/* agent communication END */

/* agent communication BEGIN (by ABLA, Abdel Rahman) */
/*
#define DEFINE_DECORATOR_NODE(NAME, DESCRIPTION, VARS) \
struct NAME : public IBTLogicNode\
{ \
  static IBTLogicNode *CreateInstance() { return new NAME; } \
  ~##NAME() { freeKids(); } \
  NAME() : IBTLogicNode(#NAME, DESCRIPTION, NT_Decorator){} \
  NAME(std::string name) : IBTLogicNode(name, DESCRIPTION, NT_Decorator){} \
  NodeStatus UpdateLogic(BT_UPDATE_FUNC_PARAMS); \
  VARS \
};
*/

#define DEFINE_DECORATOR_NODE(NAME, DESCRIPTION, VARS) \
struct NAME : public IBTLogicNode\
{ \
  static IBTLogicNode *CreateInstance() { return new NAME; } \
  ~##NAME() { freeKids(); } \
  NAME() : IBTLogicNode(#NAME, DESCRIPTION, NT_Decorator,#NAME){} \
  NAME(std::string name) : IBTLogicNode(name, DESCRIPTION, NT_Decorator,#NAME){} \
  NodeStatus UpdateLogic(BT_UPDATE_FUNC_PARAMS); \
  void OnMessageReceived(MSG_Name name, objectID self, MSG_Data data);\
  float startTime;\
  int count;\
  VARS \
};
/* agent communication END */

#define MEMBER_VAR(TYPE, NAME) TYPE NAME;
#undef BEHAVIOR_TREE_NODES_H
#include "BehaviorTreeNodes.h"

#undef LOGIC_UPDATE_FUNC
#undef LEAF_UPDATE_FUNC
#undef MEMBER_VAR
#define MEMBER_VAR(X, Y)

#define LOGIC_UPDATE_FUNC( CLASS ) \
  NodeStatus CLASS::UpdateLogic(BT_UPDATE_FUNC_PARAMS) \
{

#define END_LOGIC_UPDATE_FUNC \
  return currentStatus; \
}

#define LEAF_UPDATE_FUNC( X ) \
  NodeStatus X::UpdateLogic(BT_UPDATE_FUNC_PARAMS) \
{
#define END_LEAF_UPDATE_FUNC \
  return currentStatus; \
}
#define ON_EDIT_FUNC( X ) \
  void X::OnEdit() \
{
#define END_ON_EDIT_FUNC \
}


/* agent communication BEGIN (by ABLA, Abdel Rahman) */
#define NODE_MSG_RECEIVED( X ) \
  void X::OnMessageReceived(MSG_Name name, objectID self, MSG_Data data) \
{
#define END_NODE_MSG_RECEIVED \
  return; \
}
/* agent communication END */

IBTLogicNode * createLogicNode(std::string name);
IBTNode * createLeafNode(std::string name);
IBTLogicNode * createLogicNode(std::string name, std::string varname);
IBTNode * createLeafNode(std::string name, std::string varname);

std::unordered_map<std::string, LogicNodeInfo>& LogicNodesMap();
std::unordered_map<std::string, LeafNodeCreateFn>& LeafNodesMap();

void GenerateNodesFiles();


