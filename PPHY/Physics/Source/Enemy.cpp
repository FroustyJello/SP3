#include "Enemy.h"



Enemy::Enemy():prevX(0.f)
{
	//this->type = typeValue;
	this->scale = Vector3(1, 1, 1);
	this->mass = 1.f;
	this->momentOfInertia = 1.f;
	this->angularVelocity = 0.f;
}

Enemy::~Enemy()
{

}

void Enemy::Update(double dt, Vector3 PlayerRef, std::vector<Enemy*> m_enemies)
{
	switch (this->type)
	{
	case(GameObject::GO_ENEMY_MELEE):
		this->EnemyType = MELEE;
		break;
	case(GameObject::GO_ENEMY_MELEE_2):
		this->EnemyType = MELEE;
		break;
	case(GameObject::GO_ENEMY_MELEE_3):
		this->EnemyType = MELEE;
		break;
	case(GameObject::GO_ENEMY_MELEE_4):
		this->EnemyType = MELEE;
		break;
	case(GameObject::GO_ENEMY_RANGED):
		this->EnemyType = RANGED;
		break;
	case(GameObject::GO_ENEMY_RANGED_2):
		this->EnemyType = RANGED;
		break;
	case(GameObject::GO_BOSS_1):
		this->EnemyType = BOSS_1;
		break;
	case(GameObject::GO_BOSS_2):
		this->EnemyType = BOSS_2;
		break;
	}

	switch (this->EnemyType)
	{
	case(MELEE):
	{
		// Variable Update
		attackBT--;
		this->PlayerRef = PlayerRef;

		// Continous changing/checking of which Enemy is the closest ally
		for (std::vector<Enemy *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
		{
			Enemy *enemy = (Enemy *)*it;
			if (!enemy->active)
				continue;
			if (enemy->type == Enemy::GO_ENEMY_MELEE || enemy->type == Enemy::GO_ENEMY_RANGED)
			{
				if (this->pos != enemy->pos)
				{
					if (ClosestEnemy == NULL)
					{
						std::cout << "Assign" << std::endl;
						ClosestEnemy = enemy;
					}
					else if ((this->pos - enemy->pos).Length() < (this->pos - ClosestEnemy->pos).Length())
					{
						std::cout << "Change!" << std::endl;
						ClosestEnemy = enemy;
					}
				}
			}
		}

		// Enemy to Player Detection
		if ((this->pos - PlayerRef).Length() <= 40)
		{
			if (this->pos.y + 10 > PlayerRef.y && this->pos.y - 10 < PlayerRef.y)
			{
				DetectedPlayer = true;
			}
		}

		// Enemy to Enemy Detection
		if (ClosestEnemy != NULL)
		{
			if (DetectedPlayer)
			{
				if ((this->pos - ClosestEnemy->pos).Length() >= 30)
				{
					if (this->pos.x < ClosestEnemy->pos.x)
					{
						this->pos.x += 12.5f * dt;
					}
					else
					{
						this->pos.x -= 12.5f * dt;
					}
				}
			}
		}

		// Enemy Action
		if (DetectedPlayer)
		{
			if ((this->pos - PlayerRef).Length() > 12)
			{
				if (ClosestEnemy != NULL)
				{
					if (((this->pos + 12.5f * dt) - ClosestEnemy->pos).Length() > 12)
					{
						if (this->pos.x < PlayerRef.x)
							this->pos.x += 12.5f * dt;
					}
					if (((this->pos - 12.5f * dt) - ClosestEnemy->pos).Length() > 12)
					{
						if (this->pos.x > PlayerRef.x)
							this->pos.x -= 12.5f * dt;
					}
				}
				else
				{
					if (this->pos.x < PlayerRef.x)
					{
						this->pos.x += 12.5f * dt;
						
					}
					else
					{
						this->pos.x -= 12.5f * dt;
						
					}
				}
			}
			else if ((this->pos - PlayerRef).Length() <= 12)
			{
				if (attackBT <= 0)
				{
					attackBT = 60.f;
					std::cout << "Attack. (Melee)" << std::endl;
				}
			}
		}
	}

	if (this->pos.x < prevX)
	{
		SetAnimationStatus(true, true, dt);
	}
	else if (this->pos.x > prevX)
	{
		SetAnimationStatus(false, true, dt);
	}
	else
	{
		SetAnimationStatus(true, false, dt);
	}
	prevX = this->pos.x;

		break;
	case(RANGED):
	{
		// Variable Update
		attackBT--;
		this->PlayerRef = PlayerRef;

		// Continous changing/checking of which Enemy is the closest ally
		for (std::vector<Enemy *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
		{
			Enemy *enemy = (Enemy *)*it;
			if (!enemy->active)
				continue;
			if (enemy->type == Enemy::GO_ENEMY_MELEE || enemy->type == Enemy::GO_ENEMY_RANGED)
			{
				if (this->pos != enemy->pos)
				{
					if (ClosestEnemy == NULL)
					{
						std::cout << "Assign" << std::endl;
						ClosestEnemy = enemy;
					}
					else if ((this->pos - enemy->pos).Length() < (this->pos - ClosestEnemy->pos).Length())
					{
						std::cout << "Change!" << std::endl;
						ClosestEnemy = enemy;
					}
				}
			}
		}

		// Enemy to Player Detection
		if ((this->pos - PlayerRef).Length() <= 40)
		{
			if (this->pos.y + 10 > PlayerRef.y && this->pos.y - 10 < PlayerRef.y)
			{
				DetectedPlayer = true;
			}
		}

		// Enemy to Enemy Detection
		if (ClosestEnemy != NULL)
		{
			if (DetectedPlayer)
			{
				if ((this->pos - ClosestEnemy->pos).Length() >= 30)
				{
					if (this->pos.x < ClosestEnemy->pos.x)
						this->pos.x += 12.5f * dt;
					else
						this->pos.x -= 12.5f * dt;
				}
			}
		}

		// Enemy Action
		if (DetectedPlayer)
		{
			if ((this->pos - PlayerRef).Length() > 30)
			{
				if (ClosestEnemy != NULL)
				{
					if (((this->pos + 12.5f * dt) - ClosestEnemy->pos).Length() > 12)
					{
						if (this->pos.x < PlayerRef.x)
							this->pos.x += 12.5f * dt;
					}
					if (((this->pos - 12.5f * dt) - ClosestEnemy->pos).Length() > 12)
					{
						if (this->pos.x > PlayerRef.x)
							this->pos.x -= 12.5f * dt;
					}
				}
				else
				{
					if (this->pos.x < PlayerRef.x)
						this->pos.x += 12.5f * dt;
					else
						this->pos.x -= 12.5f * dt;
				}
			}
			else
			{
				if (attackBT <= 0)
				{
					attackBT = 60.f;
					std::cout << "Attack. (Ranged)" << std::endl;
				}
			}
		}
	}
	if (this->pos.x < prevX)
	{
		SetAnimationStatus(true, true, dt);
	}
	else if (this->pos.x > prevX)
	{
		SetAnimationStatus(false, true, dt);
	}
	else
	{
		SetAnimationStatus(true, false, dt);
	}
	prevX = this->pos.x;
		break;
	case(BOSS_1):
		break;
	case(BOSS_2):
		break;
	}
}
