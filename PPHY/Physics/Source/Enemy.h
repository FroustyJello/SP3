#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"
#include <vector>

enum EnemyType
{
	RANGED,
	MELEE,
};

class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(Vector3 pos, std::vector<Enemy> EnemyListRef, EnemyType Type);
	~Enemy();
	
	void Update(double dt, Vector3 PlayerRef);
	void MoveAndAttack(double dt);
	void LookForAllies(double dt);
	bool HasAllies();

private:
	std::vector<Enemy> EnemyListRef;
	EnemyType Type;
	Vector3 PlayerRef;
};
#endif // !ENEMY_H
