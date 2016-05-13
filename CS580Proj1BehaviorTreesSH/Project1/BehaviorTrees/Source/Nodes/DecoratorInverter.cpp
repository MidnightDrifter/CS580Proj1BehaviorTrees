#include "DXUT.h"
#include "../BehaviorTree.h"
LOGIC_UPDATE_FUNC(DecoratorInverter)
{
		if (currentStatus == NS_OnEnter)
		{
			currentStatus = NS_Running;
			m_currentChildIndex = 0;
		}
		else
		{
			if (childStatus == NS_Running)
			{
				currentStatus = NS_Running;
			}
			else if (childStatus == NS_Completed)
			{
				currentStatus = NS_Failed;
			}
			else if (childStatus == NS_Failed)
			{
				currentStatus = NS_Completed;
			}
		}
}
END_LOGIC_UPDATE_FUNC
ON_EDIT_FUNC(DecoratorInverter)
{

}
END_ON_EDIT_FUNC
NODE_MSG_RECEIVED(DecoratorInverter)
{

}
END_NODE_MSG_RECEIVED