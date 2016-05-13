#include "DXUT.h"
#include "Helpers.h"
#include "database.h"
#include "body.h"

std::set<objectID> Agents::agents[count];
int   Params::MinFishInGroup = 3;
float Params::MaxFishDist = 0.7f;
float Params::MinGroupDist = 0.4f;
bool isType(AgentType t, objectID id)
{
  return Agents::agents[t].find(id) != Agents::agents[t].end();
}

bool isType(AgentType t, GameObject*ptr)
{
  return ptr && Agents::agents[t].find(ptr->GetID()) != Agents::agents[t].end();
}

GroupInfo getGroup(GameObject* obj)
{
  
  // a fish is in a group if there are MinGroup fish MinGroupDist away from it
  // fish in a group can not be targeted or eaten.
  std::vector<GameObject*> group;
  for (auto id : Agents::agents[Fish])
  {
    GameObject * i = g_database.Find(id);
    if (i && (i)->GetID() != obj->GetID())
    {
      D3DXVECTOR3 npcPos = (i)->GetBody().GetPos();
      D3DXVECTOR3 myPos = obj->GetBody().GetPos();
      D3DXVECTOR3 diff = npcPos - myPos;
      float distance = D3DXVec3Length(&diff);

      if (distance < Params::MinGroupDist)
      {
        group.push_back(i);
      }
    }
  }
  // determine center
  GroupInfo info = { group.size() > Params::MinFishInGroup, { 0, 0, 0 } };
  if (info.ingroup)
  {
    for (auto f_ptr : group)
    {
      info.center += f_ptr->GetBody().GetPos();
    }
    info.center *= 1.0f / group.size();
  }
  return info;
}

float getPerimeterRadius()
{
  // get the largest position of any fish
  float farthest = 0.0f;
  for (auto id : Agents::agents[Fish])
  {
    GameObject * i = g_database.Find(id);
    if (!i) continue;
    D3DXVECTOR3 npcPos = (i)->GetBody().GetPos();
    D3DXVECTOR3 myPos = { 0.5f, 0.0f, 0.5f };
    D3DXVECTOR3 diff = npcPos - myPos;
    float distance = D3DXVec3Length(&diff);
    if (distance > farthest)
    {
      farthest = distance;
    }
  }
  return farthest;
}

DangerLevel getDangerLevel(GameObject * me,GameObject* shark)
{
  if (shark && isType(Shark,shark))
  {
    D3DXVECTOR3 dir = shark->GetBody().GetPos() - me->GetBody().GetPos();
    float dist = D3DXVec3Length(&dir);
    if (dist > 0.34f)
    {
      return SAFE;
    }
    if (dist > 0.2f)
    {
      return CLOSE;
    }
  }
  return SAFE;
}

GameObject* getClosestAgent(GameObject*me, AgentType t)
{
  float closestDist = 0.0f;
  GameObject* farthestGameObject = nullptr;

  for (auto id : Agents::agents[t])
  {
    GameObject * i = g_database.Find(id);
    if (i && (i)->GetID() != me->GetID())
    {
      D3DXVECTOR3 npcPos = (i)->GetBody().GetPos();
      D3DXVECTOR3 myPos = me->GetBody().GetPos();
      D3DXVECTOR3 diff = npcPos - myPos;
      float distance = D3DXVec3Length(&diff);

      if (farthestGameObject)
      {
        if (distance < closestDist)
        {
          closestDist = distance;
          farthestGameObject = i;
        }
      }
      else
      {
        closestDist = distance;
        farthestGameObject = i;
      }
    }
  }

  return farthestGameObject;
}

GameObject* getClosestAgentNot(GameObject*me, AgentType t)
{
  float closestDist = 0.0f;
  GameObject* farthestGameObject = 0;
  dbCompositionList list;
  g_database.ComposeList(list, OBJECT_NPC);

  dbCompositionList::iterator i;
  for (i = list.begin(); i != list.end(); ++i)
  {
    if ((*i)->GetID() != me->GetID() && !isType(t,*i))
    {
      D3DXVECTOR3 npcPos = (*i)->GetBody().GetPos();
      D3DXVECTOR3 myPos = me->GetBody().GetPos();
      D3DXVECTOR3 diff = npcPos - myPos;
      float distance = D3DXVec3Length(&diff);

      if (farthestGameObject)
      {
        if (distance < closestDist)
        {
          closestDist = distance;
          farthestGameObject = *i;
        }
      }
      else
      {
        closestDist = distance;
        farthestGameObject = *i;
      }
    }
  }
  return farthestGameObject;
}

GroupInfo getClosestGroup(GameObject*me)
{
  float closestDist = 0.0f;
  GroupInfo closestgroup = { false };

  for (auto id : Agents::agents[Fish])
  {
    GameObject * i = g_database.Find(id);

    if (i && (i)->GetID() != me->GetID() && getGroup(i).ingroup)
    {
      D3DXVECTOR3 npcPos = (i)->GetBody().GetPos();
      D3DXVECTOR3 myPos = me->GetBody().GetPos();
      D3DXVECTOR3 diff = npcPos - myPos;
      float distance = D3DXVec3Length(&diff);

      if (closestgroup.ingroup)
      {
        if (distance < closestDist)
        {
          closestDist = distance;
          closestgroup = getGroup(i);
        }
      }
      else
      {
        closestDist = distance;
        closestgroup = getGroup(i);
      }
    }
  }
  return closestgroup;
}
