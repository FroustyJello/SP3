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
	bool IsShooting = false;

private:
	// Enemy Variables
	float attackBT = 0.f; // Attack bounce time
	float prevX;
	float m_timer;
	float m_timer2;
	Enemy *ClosestEnemy = NULL;
	bool DetectedPlayer = false;
	EnemyType EnemyType;
	CCollider *LeftSight = new CCollider;
	CCollider *RightSight = new CCollider;

	bool RL;
	bool isAttacking;
	bool CanAttack;

	// Boss Variables
	int BossAttacks; // 1 = Punch, 2 = Laser, 3 = Dash Punch
	Vector3 LastLocation;
};
#endif // !ENEMY_H
