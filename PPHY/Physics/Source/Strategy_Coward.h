#pragma once
#include "Strategy.h"
#include "Vector3.h"

class CStrategy_Coward
	: public CStrategy
{
public:
	CStrategy_Coward(void);
	~CStrategy_Coward(void);

	void Update(Vector3& theDestination, Vector3& theEnemyPosition);
	void SetDestination(const float x, const float y);

	int GetDestination_x(void);
	int GetDestination_y(void);
	Vector3 GetDestination(void);

	// The AI states of the enemy
	enum CURRENT_STATE
	{
		IDLE,
		AVOID,
		RUN,
		NUM_ENEMY_STATE,
	};
	enum AI_STATE_RANGE
	{
		AI_STATE_AVOID = 125,
		AI_STATE_RUN = 25,
		NUM_AI_STATE_RANGE,
	};
	CStrategy_Coward::CURRENT_STATE GetState(void);
	void SetState(CStrategy_Coward::CURRENT_STATE);

private:
	// Enemy AI State
	CStrategy_Coward::CURRENT_STATE CurrentState;
	bool reverse;
	float idle_max;
};
