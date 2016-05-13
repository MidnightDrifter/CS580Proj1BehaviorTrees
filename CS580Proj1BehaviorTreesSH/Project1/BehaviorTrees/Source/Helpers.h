#pragma once
#include <set>
#include "gameobject.h"

enum AgentType
{
  Fish,
  Shark,
  Bird,
  count
};
struct Agents
{
  static std::set<objectID> agents[AgentType::count];
};

struct Params
{
  static int MinFishInGroup;
  static float MaxFishDist;
  static float MinGroupDist;
};

bool isType(AgentType t, objectID);
bool isType(AgentType t, GameObject *);

struct GroupInfo
{
  bool ingroup;
  D3DXVECTOR3 center;
};
GroupInfo getGroup(GameObject * obj);

float getPerimeterRadius();

enum DangerLevel
{
  SAFE,
  CLOSE,
  DANGER
};
DangerLevel getDangerLevel(GameObject * me ,GameObject * shark);

GameObject * getClosestAgent(GameObject *, AgentType t);
GameObject * getClosestAgentNot(GameObject *, AgentType t);
GroupInfo  getClosestGroup(GameObject *);