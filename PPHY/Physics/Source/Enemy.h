#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"
#include "Animation.h"
#include "Player.h"
#include <vector>

enum EnemyType
{
	RANGED,
	MELEE,
	BOSS_1,
	BOSS_2,
};

class Enemy : public GameObject, public CAnimation
{
public:
	Enemy();
	//Enemy(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~Enemy();
	
	void Update(double dt, CPlayer *PlayerRef, std::vector<Enemy *> m_enemies, std::vector<CCollider*> m_Collider);

private:
	// Enemy Variables
	float attackBT = 0.f; // Attack bounce time
	float prevX;
	Enemy *ClosestEnemy = NULL;
	bool DetectedPlayer = false;
	EnemyType EnemyType;
	CCollider *LeftSight = new CCollider;
	CCollider *RightSight = new CCollider;

	// Boss_1 Variables
	int Boss_1_Attack = 0;
	//bool isAttacking = false;

	bool RL;
	bool isAttacking;

	// Boss_1 Variables
	Vector3 LastPlayerPosition;

};
#endif // !ENEMY_H
