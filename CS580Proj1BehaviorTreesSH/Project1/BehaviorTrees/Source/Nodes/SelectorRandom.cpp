#include "DXUT.h"
#include "../BehaviorTree.h"
LOGIC_UPDATE_FUNC(SelectorRandom)
{
  if (currentStatus == NS_OnEnter)
  {
    m_currentChildIndex = rand() % m_children.size();
    currentStatus = NS_Running;
  }
  else
  {
    if (childStatus == NS_Failed)
    {
      m_currentChildIndex = rand() % m_children.size();
    }
    else if (childStatus == NS_Completed)
    {
      currentStatus = NS_Completed;
    }
    else if (childStatus == NS_Running)
    {
      currentStatus = NS_Running;
    }
  }
}
END_LOGIC_UPDATE_FUNC
ON_EDIT_FUNC(SelectorRandom)
{

}
END_ON_EDIT_FUNC
NODE_MSG_RECEIVED(SelectorRandom)
{

}
END_NODE_MSG_RECEIVED