#include "DXUT.h"
#include "../BehaviorTree.h"
LOGIC_UPDATE_FUNC(Sequencer)
{
	if (currentStatus == NS_OnEnter)
	{
		currentStatus = NS_Running;
		m_currentChildIndex = 0;
	}
	else
	{
		if (childStatus == NS_Completed)
		{
			m_currentChildIndex++;

			if (m_currentChildIndex >= (int)m_children.size())
			{
				currentStatus = NS_Completed;
			}
		}
		else if (childStatus == NS_Failed)
		{
			currentStatus = NS_Failed;
		}
	}
}
END_LOGIC_UPDATE_FUNC

ON_EDIT_FUNC(Sequencer)
{
  
}
END_ON_EDIT_FUNC

NODE_MSG_RECEIVED(Sequencer)
{

}
END_NODE_MSG_RECEIVED