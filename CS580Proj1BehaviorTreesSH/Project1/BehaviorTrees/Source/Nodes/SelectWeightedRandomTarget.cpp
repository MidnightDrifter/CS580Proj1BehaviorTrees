#include "DXUT.h"
#include "../gameobject.h"
#include "../BehaviorTree.h"
#include "../database.h"
#include "../body.h"
#include "../movement.h"
LEAF_UPDATE_FUNC(SelectRandomTarget)
{
  GameObject *me = g_database.Find(self);
  if (me)
  {
    D3DXVECTOR3 target(0, 0, 0);
    target.x = (float)(rand() % 256) / 256.f;
    target.z = (float)(rand() % 256) / 256.f;
    me->SetTargetPOS(target);
    currentStatus = NS_Completed;
  }
  else
  {
    return NS_Failed;
  }
}
END_LEAF_UPDATE_FUNC
ON_EDIT_FUNC(SelectRandomTarget)
{

}
END_ON_EDIT_FUNC
NODE_MSG_RECEIVED(SelectRandomTarget)
{

}
END_NODE_MSG_RECEIVED