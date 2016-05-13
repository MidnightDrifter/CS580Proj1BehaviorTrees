/* Copyright Steve Rabin, 2013. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2013"
 */

#include "DXUT.h"
#include <sstream>
#include "gameobject.h"
#include "msgroute.h"
#include "statemch.h"
#include "movement.h"
#include "body.h"
#include "BehaviorTree.h"

GameObject::GameObject( objectID id, unsigned int type, char* name )
: m_markedForDeletion(false),
  m_stateMachineManager(0),
  m_body(0),
  m_movement(0),
  m_tiny(0),
  m_behaviorTree(0),
  m_targetPOS(0,0,0)
{
	m_id = id;
	m_type = type;
	
	if( strlen(name) < GAME_OBJECT_MAX_NAME_SIZE ) {
		strcpy( m_name, name );
	}
	else {
		strcpy( m_name, "invalid_name" );
		ASSERTMSG(0, "GameObject::GameObject - name is too long" );
	}
}

GameObject::~GameObject( void )
{
	if(m_stateMachineManager)
	{
		delete m_stateMachineManager;
	}
	if(m_body)
	{
		delete m_body;
	}
	if(m_movement)
	{
		delete m_movement;
	}
	if(m_tiny)
	{
		delete m_tiny;
	}
	FreeTree();
}

void GameObject::CreateStateMachineManager( void )
{
  ASSERTMSG(!HasBehaviorTree(), "GameObject::CreateStateMachine: Tried to add a state machine to an agent that already has a behavior tree.");
  ASSERTMSG(!HasStateMachine(), "GameObject::CreateStateMachine: Tried to add a state machine to an agent that already has a state machine.");
	m_stateMachineManager = new StateMachineManager( *this );
}

void GameObject::CreateMovement( void )
{
	m_movement = new Movement( *this ); 
}

void GameObject::CreateBody( int health, D3DXVECTOR3& pos )
{
	m_body = new Body( health, pos, *this );
}

void GameObject::CreateTiny( CMultiAnim *pMA, std::vector< CTiny* > *pv_pChars, CSoundManager *pSM, double dTimeCurrent, float red, float green, float blue )
{
    m_tiny = new CTiny( *this );
    if( m_tiny == NULL )
	{
		return;
	}

    if( SUCCEEDED( m_tiny->Setup( pMA, pv_pChars, pSM, dTimeCurrent ) ) )
	{
        m_tiny->SetSounds( true );
		m_tiny->SetDiffuse(red, green, blue);
	}
	else
	{
		delete m_tiny;
	}
}

void GameObject::Initialize( void )
{

}

/*---------------------------------------------------------------------------*
  Name:         Update

  Description:  Calls the update function of the currect state machine.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void GameObject::Update( void )
{
	if(m_stateMachineManager)
	{
		m_stateMachineManager->Update();
	}
}

void GameObject::Animate( double dTimeDelta )
{
  if (DXUTIsTimePaused()) return;

	if( m_movement )
	{
		m_movement->Animate( dTimeDelta );
	}

	if( m_behaviorTree )
	{
		m_behaviorTree->Update( m_id, dTimeDelta );
	}
}

void GameObject::AdvanceTime( double dTimeDelta, D3DXVECTOR3 *pvEye )
{
	if( m_tiny )
	{
		m_tiny->AdvanceTime( dTimeDelta, pvEye );
	}
}

void GameObject::Draw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj )
{
	if( m_tiny )
	{
		m_tiny->Draw();
	}
}

void GameObject::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_tiny )
	{
		m_tiny->RestoreDeviceObjects( pd3dDevice );
	}
}

void GameObject::InvalidateDeviceObjects( void )
{

}

int GetDepth(std::string &line)
{
	int depth = 0;

	for (unsigned int i = 0; i < line.size(); ++i)
	{
		if (line[i] == '\t')
		{
			depth++;
		}
		else
		{
			break;
		}
	}

	return depth;
}
IBTNode* GameObject::GetBehaviorTree()
{
  ASSERTMSG(m_behaviorTree, "GameObject::GetBehaviorTree - m_behaviorTree not set"); 
  return(m_behaviorTree);
}

void GameObject::CreateBehaviorTree(std::string TreeName)
{
  ASSERTMSG(!HasBehaviorTree(), "GameObject::CreateBehaviorTree: Tried to add a state machine to an agent that already has a behavior tree.");
  ASSERTMSG(!HasStateMachine(), "GameObject::CreateBehaviorTree: Tried to add a state machine to an agent that already has a state machine.");
	
  GenerateTree("BTResources/" + TreeName + ".bhv");
}

void GameObject::GenerateTreeRecursive(IBTLogicNode *parent, int depth, std::string &line, std::ifstream &input)
{
	IBTLogicNode *last = 0;

	do
	{
	startLoop:
		int currentDepth = GetDepth(line);

		std::stringstream ss;

		ss.str(line);

		std::string token;
		std::string nodeName;
		std::string nodeType;

		do
		{
			getline(ss, token, '\t');
		} while (token == "");

		std::stringstream ss2;
		ss2.str(token);

		ss2 >> nodeName;
		ss2 >> nodeType;

		if (currentDepth)
		{
			if (currentDepth == depth)
			{
				if (nodeType == "Leaf")
				{
					IBTNode *node = createLeafNode(nodeName, nodeName);
					parent->m_children.push_back(node);
					node->p_parent = parent;
					last = 0;
				}
				else
				{
					IBTLogicNode* node = createLogicNode(nodeType, nodeName);
					parent->m_children.push_back(node);
					node->p_parent = parent;
					last = node;
				}
			}
			else if (currentDepth == depth + 1)
			{
				GenerateTreeRecursive(last, currentDepth, line, input);

				if (currentDepth == 0)
				{
					return;
				}
				else
				{
					if (line == "")
					{
						return;
					}
					else
					{
						goto startLoop;
					}
				}
			}
			else if (currentDepth < depth)
			{
				return;
			}
		}
		else
		{
			m_behaviorTree = createLogicNode(nodeType, nodeName);
			last = m_behaviorTree;
			m_behaviorTree->p_parent = 0;
		}
	} while (getline(input, line));

	line = "";
}

void GameObject::GenerateTree(const std::string filename)
{
  treename = filename;
	//Delete root if it exists
	FreeTree();

	std::ifstream input(filename);

	std::string line;
	getline(input, line);

	GenerateTreeRecursive(0, 0, line, input);

	input.close();
}

void GameObject::FreeTree()
{
  if (m_behaviorTree) 
  { 
    delete m_behaviorTree; 
    m_behaviorTree = 0; 
  }
}


void GameObject::SaveBHVRecursive(IBTNode* node, int depth, std::ostream & out)
{
  // print tabs
  for (int i = 0; i < depth; ++i)
  {
    out << "\t";
  }
  // print parent name and type
  std::string type;
  switch (node->m_type)
  {
  case NT_Selector:type = "Selector"; break;
  case NT_Decorator:type = "Decorator"; break;
  case NT_Leaf:type = "Leaf"; break;
  default: break;
  }

  if (node->m_type == NT_Leaf)
  {
    type = "Leaf";
    out << node->m_type_name << " " << "Leaf" << std::endl;
  }
  else
  {
    out << node->m_name << " " << node->m_type_name << std::endl;
  }


  // if it is a parent...
  if (node->m_type != NT_Leaf)
  {
    IBTLogicNode * parent = static_cast<IBTLogicNode *>(node);
    // for each child, print them out with depth + 1
    for (auto & child : parent->m_children)
    {
      SaveBHVRecursive(child, depth + 1, out);
    }
  }
}

void GameObject::SaveAsBHV(const std::string& BehaviorName)
{
  std::string filename = "BTResources/" + BehaviorName + ".bhv";

  std::ofstream out;
  out.open(filename);
  if (out.good())
  {
    // write to the file
    SaveBHVRecursive(GetBehaviorTree(), 0, out);

    out.close();
  }
  // switch to using that name
  SetTreeName(BehaviorName);
}

void GameObject::SaveBHV()
{
  SaveAsBHV(treename);
}

void GameObject::SetTreeName(const std::string& name)
{
  treename = name;
}
