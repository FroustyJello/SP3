#pragma once
#include "Vector3.h"
#include "Strategy.h"
#include "GameObject.h"

class CEnemy : public GameObject
{
//	friend class CStrategy;
public:
	CEnemy(void);
	~CEnemy(void);

	// Initialise this class instance
	void Init();
	// Set the destination of this enemy
	void SetDestination(Vector3 pos);

	// Get position x of the player
	int GetPos_x(void) const;
	// Get position y of the player
	int GetPos_y(void) const;
	// Get position of the player in Vector3 form
	Vector3 GetPos(void) const;
	// Get the destination of this enemy
	int GetDestination_x(void) const;
	// Get the destination of this enemy
	int GetDestination_y(void) const;
	// Get the destination of this enemy as a Vector3
	Vector3 GetDestination(void) const;

	// ENEMY Update
	void Update(void);

	// Strategy
	void ChangeStrategy(CStrategy* theNewStrategy, bool bDelete=true);

private:
	// The Destination is the position of the Hero
	Vector3 theDestination;

	CStrategy* theStrategy;


};
