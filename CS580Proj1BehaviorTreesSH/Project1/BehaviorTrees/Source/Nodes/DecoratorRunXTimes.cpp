#include "DXUT.h"
#include "DecoratorRunXTimes.h"
LOGIC_UPDATE_FUNC(DecoratorRunXTimes)
{
	if (currentStatus == NS_OnEnter)
	{
		currentStatus = NS_Running;
		currX = 0;
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
			//currentStatus = NS_Failed;
			if (currX < x)
			{
				(currX)++;
				//[tell child to run again here?]
			}

			else
			{
				currentStatus = NS_Completed;
			}

		}
		else if (childStatus == NS_Failed)
		{
			//currentStatus = NS_Completed;

			//currentStatus = NS_Failed;
			if (currX < x)
			{
				(currX)++;
				//[tell child to run again here?]
				currentStatus = NS_Running;
			}

			else
			{
				childStatus = NS_Failed;
			}

		}
	}
}