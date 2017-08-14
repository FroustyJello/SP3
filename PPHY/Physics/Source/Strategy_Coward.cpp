#include "Strategy_Coward.h"
#include <iostream>

using namespace std;

/********************************************************************************
Constructor
********************************************************************************/
CStrategy_Coward::CStrategy_Coward()
{
	reverse = false;
	idle_max = 0;
}

/********************************************************************************
Destructor
********************************************************************************/
CStrategy_Coward::~CStrategy_Coward()
{
}

/********************************************************************************
Update method
********************************************************************************/
void CStrategy_Coward::Update(Vector3& theDestination, Vector3& theEnemyPosition)
{
	// Decide which state to change to
	int distanceHeroToEnemy = CalculateDistance(theDestination, theEnemyPosition);
	if (distanceHeroToEnemy < AI_STATE_AVOID*AI_STATE_AVOID)
	{
		if (distanceHeroToEnemy < AI_STATE_RUN*AI_STATE_RUN)
			CurrentState = RUN;
		else
			CurrentState = AVOID;
	}
	else
		CurrentState = IDLE;

	// Based on the current state, move the enemy
	switch (CurrentState)
	{
	case AVOID:
		theEnemyPosition.x = theEnemyPosition.x + (theDestination.x - theEnemyPosition.x > 0 ? -2 : 2);
		break;
	case RUN:
		theEnemyPosition.x = theEnemyPosition.x + (theDestination.x - theEnemyPosition.x > 0 ? -5 : 5);
		break;
	case IDLE:
		// Do nothing if idling
		if (idle_max >= 25)
			reverse = true;
		else if (idle_max <= 0)
			reverse = false;

		if (reverse == false)
		{
			idle_max += 0.8f;
			theEnemyPosition.x += 0.8f;
		}
		else
		{
			idle_max -= 0.8f;
			theEnemyPosition.x -= 0.8f;
		}
		break;
	}
}

/********************************************************************************
Set the destination for this strategy
********************************************************************************/
void CStrategy_Coward::SetDestination(const float x, const float y)
{
	theDestination.x = x;
	theDestination.y = y;
}

/********************************************************************************
Get the destination for this strategy
********************************************************************************/
int CStrategy_Coward::GetDestination_x(void)
{
	return theDestination.x;
}

/********************************************************************************
Get the destination for this strategy
********************************************************************************/
int CStrategy_Coward::GetDestination_y(void)
{
	return theDestination.y;
}

/********************************************************************************
Get the destination for this strategy
********************************************************************************/
Vector3 CStrategy_Coward::GetDestination(void)
{
	return theDestination;
}

/********************************************************************************
Get the FSM state for this strategy
********************************************************************************/
CStrategy_Coward::CURRENT_STATE CStrategy_Coward::GetState(void)
{
	return CurrentState;
}

/********************************************************************************
Set the FSM state for this strategy
********************************************************************************/
void CStrategy_Coward::SetState(CStrategy_Coward::CURRENT_STATE theEnemyState)
{
	CurrentState = theEnemyState;
}
