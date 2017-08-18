#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"
#include <vector>

enum EnemyType
{
	RANGED,
	MELEE,
	BOSS_1,
	BOSS_2,
};

class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~Enemy();
	
	void Update(double dt, Vector3 PlayerRef, std::vector<Enemy *> m_enemies);

private:
	// Enemy Variables
	float attackBT = 0.f; // Attack bounce time
	Enemy *ClosestEnemy = NULL;
	bool DetectedPlayer = false;
	EnemyType EnemyType;

	// Boss_1 Variables
	int Boss_1_Attack = 0;
	bool isAttacking = false;
	Vector3 LastPlayerPosition;
};
#endif // !ENEMY_H
