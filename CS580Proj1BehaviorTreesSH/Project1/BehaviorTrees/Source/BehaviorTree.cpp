//	File name:		BehaviorTree.cpp
//	Author(s):		Allan Deutsch
//	
//	All content 2014-2015 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "DXUT.h"
#include <unordered_map>
#include <fstream>
#include "BehaviorTree.h"
#include "global.h"
#include "database.h"
#include "gameobject.h"

void PushText(objectID id, string text)
{
  GameObject *me = g_database.Find(id);

  me->m_behaviorTreeInfo << ">" << text;

  if (ShortTextOn)
  {
    // get index of cutoff point
    string aistring = "...";
    string test = me->m_behaviorTreeInfo.str();
    int length = me->m_behaviorTreeInfo.str().length();
    int delimitcount = 0;
    for (int i = length - 1; i >= 0; --i)
    {
      if (me->m_behaviorTreeInfo.str()[i] == '>')
      {
        ++delimitcount;
      }

      if (delimitcount == 2 && i != 0) // crop remaining string at beginning
      {
        aistring += me->m_behaviorTreeInfo.str().substr(i);
        me->m_behaviorTreeInfo.str("");
        me->m_behaviorTreeInfo << aistring;
        break;
      }
    }
  }

}

NodeStatus IBTNode::Update(BT_UPDATE_FUNC_PARAMS)
{

  if (currentStatus == NS_Completed || currentStatus == NS_Failed)
  {
    currentStatus = NS_OnEnter;
  }
  if (DEBUG_BT)
  {
    PushText(self, m_name);
  }
  
  currentStatus = UpdateLogic(BT_UPDATE_FUNC_PARAMS_UNDECORATED);

  return currentStatus;
}

NodeStatus IBTLogicNode::Update(BT_UPDATE_FUNC_PARAMS)
{
  if (DEBUG_BT)
    PushText(self, m_name);
  if (m_currentChildIndex != -1)
  {
    childStatus = m_children[m_currentChildIndex]->Update(BT_UPDATE_FUNC_PARAMS_UNDECORATED);
  }
  currentStatus = UpdateLogic(BT_UPDATE_FUNC_PARAMS_UNDECORATED);
  if (currentStatus == NS_Completed || currentStatus == NS_Failed)
  {
    NodeStatus retval = currentStatus;
    m_currentChildIndex = -1;
    currentStatus = NS_OnEnter;
    for (std::vector<IBTNode *>::iterator it = m_children.begin(); it != m_children.end(); ++it)
    {
      IBTNode * node = *it;
      IBTLogicNode *logic = dynamic_cast<IBTLogicNode*>(node);
      if (logic)
      {
        logic->ResetLogicStatus();
      }
      else
      {
        node->ResetStatus();
      }
    }
    return retval;
  }
  return currentStatus;
}

std::unordered_map<std::string, LogicNodeInfo> logicNodeMapGenerator()
{

  std::unordered_map<std::string, LogicNodeInfo> logicNodes;
#undef BEHAVIOR_TREE_NODES_H
#undef DEFINE_LEAF_NODE 
#undef DEFINE_SELECTOR_NODE
#undef DEFINE_DECORATOR_NODE
#define DEFINE_LEAF_NODE(X, Y, Z) 
#define DEFINE_SELECTOR_NODE(X, Y, Z) logicNodes.emplace( std::make_pair(std::string(#X), std::make_pair(NT_Selector,&X::CreateInstance) ));
#define DEFINE_DECORATOR_NODE(X, Y, Z) logicNodes.emplace( std::make_pair(std::string(#X), std::make_pair(NT_Decorator,&X::CreateInstance) ));

#include "BehaviorTreeNodes.h"

#undef BEHAVIOR_TREE_NODES_H
#undef DEFINE_SELECTOR_NODE
#undef DEFINE_DECORATOR_NODE
#undef DEFINE_LEAF_NODE 

  return logicNodes;
}
std::unordered_map<std::string, LeafNodeCreateFn> leafNodeMapGenerator()
{
  std::unordered_map<std::string, LeafNodeCreateFn> leafNodes;

#undef BEHAVIOR_TREE_NODES_H
#undef DEFINE_LEAF_NODE 
#undef DEFINE_SELECTOR_NODE
#undef DEFINE_DECORATOR_NODE
#define DEFINE_SELECTOR_NODE(X, Y, Z) 
#define DEFINE_DECORATOR_NODE(X, Y, Z) 
#define DEFINE_LEAF_NODE(X, Y, Z) leafNodes.emplace(std::make_pair(std::string(#X), &X::CreateInstance) );

#include "BehaviorTreeNodes.h"

#undef BEHAVIOR_TREE_NODES_H
#undef DEFINE_SELECTOR_NODE
#undef DEFINE_DECORATOR_NODE
#undef DEFINE_LEAF_NODE 
  
  
  return leafNodes;
}

void GenerateNodesFiles()
{
  std::ofstream NodesFile("BTResources/nodes.vbt");

  // First, output the selector nodes:
  NodesFile << "Selectors\n";
#undef BEHAVIOR_TREE_NODES_H
#undef DEFINE_LEAF_NODE 
#undef DEFINE_SELECTOR_NODE
#undef DEFINE_DECORATOR_NODE
#define DEFINE_SELECTOR_NODE(X, Y, Z) NodesFile << "\t" #X " " #Y "\n";
#define DEFINE_DECORATOR_NODE(X, Y, Z) 
#define DEFINE_LEAF_NODE(X, Y, Z)

#include "BehaviorTreeNodes.h"

#undef BEHAVIOR_TREE_NODES_H
#undef DEFINE_SELECTOR_NODE
#undef DEFINE_DECORATOR_NODE
#undef DEFINE_LEAF_NODE 

  // second, output the decorator nodes:
  NodesFile << "Decorators\n";
#undef BEHAVIOR_TREE_NODES_H
#undef DEFINE_LEAF_NODE 
#undef DEFINE_SELECTOR_NODE
#undef DEFINE_DECORATOR_NODE
#define DEFINE_SELECTOR_NODE(X, Y, Z) 
#define DEFINE_DECORATOR_NODE(X, Y, Z) NodesFile << "\t" #X " " #Y "\n";
#define DEFINE_LEAF_NODE(X, Y, Z)

#include "BehaviorTreeNodes.h"

#undef BEHAVIOR_TREE_NODES_H
#undef DEFINE_SELECTOR_NODE
#undef DEFINE_DECORATOR_NODE
#undef DEFINE_LEAF_NODE 

  // second, output the decorator nodes:
  NodesFile << "Leaves\n";
#undef BEHAVIOR_TREE_NODES_H
#undef DEFINE_LEAF_NODE 
#undef DEFINE_SELECTOR_NODE
#undef DEFINE_DECORATOR_NODE
#define DEFINE_SELECTOR_NODE(X, Y, Z) 
#define DEFINE_DECORATOR_NODE(X, Y, Z) 
#define DEFINE_LEAF_NODE(X, Y, Z) NodesFile << "\t" #X " " #Y "\n";

#include "BehaviorTreeNodes.h"

#undef BEHAVIOR_TREE_NODES_H
#undef DEFINE_SELECTOR_NODE
#undef DEFINE_DECORATOR_NODE
#undef DEFINE_LEAF_NODE 

  NodesFile.close();
}



std::unordered_map<std::string, LogicNodeInfo>& LogicNodesMap()
{
  static std::unordered_map<std::string, LogicNodeInfo> nodes = logicNodeMapGenerator();
  return nodes;
}

std::unordered_map<std::string, LeafNodeCreateFn>& LeafNodesMap()
{
  static std::unordered_map<std::string, LeafNodeCreateFn> nodes = leafNodeMapGenerator();
  return nodes;
}

IBTLogicNode * createLogicNode(std::string name)
{
  auto nodes = LogicNodesMap();
  auto it = nodes.find(name);
  if (it != nodes.end())
    return (*it).second.second();
  else
    return nullptr;

}


IBTNode * createLeafNode(std::string name)
{
  auto nodes = LeafNodesMap();
  auto it = nodes.find(name);
  if (it != nodes.end())
    return (*it).second();
  else
    return nullptr;
}

IBTLogicNode * createLogicNode(std::string name, std::string varname)
{
  std::unordered_map<std::string, LogicNodeInfo>& nodes = LogicNodesMap();
  std::unordered_map<std::string, LogicNodeInfo>::iterator it = nodes.find(name);
  if (it != nodes.end())
  {
    IBTLogicNode * node = (*it).second.second();
    *const_cast<std::string *>(&(node->m_name)) = varname;
    return node;
  }
  else
    return nullptr;
}

IBTNode * createLeafNode(std::string name, std::string varname)
{
  std::unordered_map<std::string, LeafNodeCreateFn>& nodes = LeafNodesMap();
  std::unordered_map<std::string, LeafNodeCreateFn>::iterator it = nodes.find(name);
  if (it != nodes.end())
  {
    IBTNode * node = (*it).second();
    *const_cast<std::string *>(&(node->m_name)) = varname;
    return node;
  }
  else
    return nullptr;
}
