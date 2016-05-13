#include "DXUT.h"
#include "../BehaviorTree.h"
LOGIC_UPDATE_FUNC(DecoratorRunUntilSucceed)
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
			currentStatus = NS_Completed;
		}
	}
}
END_LOGIC_UPDATE_FUNC
ON_EDIT_FUNC(DecoratorRunUntilSucceed)
{

}
END_ON_EDIT_FUNC
NODE_MSG_RECEIVED(DecoratorRunUntilSucceed)
{

}
END_NODE_MSG_RECEIVED