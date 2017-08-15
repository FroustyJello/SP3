#include "Enemy.h"

Enemy::Enemy()
{

}

Enemy::Enemy(Vector3 pos, std::vector<Enemy> EnemyListRef, EnemyType Type)
{
	this->pos = pos;
	this->EnemyListRef = EnemyListRef;
	this->Type = Type;
}

Enemy::~Enemy()
{

}

void Enemy::Update(double dt, Vector3 PlayerRef)
{
	this->PlayerRef = PlayerRef;

	if (!HasAllies())
	{
		LookForAllies(dt);
	}
	else
	{
		MoveAndAttack(dt);
	}
}

void Enemy::MoveAndAttack(double dt)
{
	if (this->Type == RANGED)
	{
		if ((this->pos - PlayerRef).Length() >= 5)
		{
			// Move Towards Player
			// Problem, Verticality(?)
			if (this->pos.x > PlayerRef.x)
			{
				this->pos.x -= 10.f * dt;
			}
			if (this->pos.x < PlayerRef.x)
			{
				this->pos.x += 10.f * dt;
			}
		}
		else
		{
			// Attack Player
			printf("Enemy Attack(Ranged)");
		}
	}
	else
	{
		if ((this->pos - PlayerRef).Length() >= 2)
		{
			// Move Towards Player
			// Problem, Verticality(?)
			if (this->pos.x > PlayerRef.x)
			{
				this->pos.x -= 10.f * dt;
			}
			if (this->pos.x < PlayerRef.x)
			{
				this->pos.x += 10.f * dt;
			}
		}
		else
		{
			// Attack Player
			printf("Enemy Attack(Melee)");
		}
	}
}

void Enemy::LookForAllies(double dt)
{
	for (int i = 0; i < EnemyListRef.size(); i++)
	{
		if (this->pos != EnemyListRef[i].pos)
		{
			// Depending On Enemy Type, Enemy's range to search for Allies increases
			if (this->type == RANGED)
			{
				// If there is an enemy within the range of 15 meters
				if ((this->pos - EnemyListRef[i].pos).Length() <= 15)
				{
					if (this->pos.x > EnemyListRef[i].pos.x)
					{
						this->pos.x -= 10.f * dt;
					}
					if (this->pos.x < EnemyListRef[i].pos.x)
					{
						this->pos.x += 10.f * dt;
					}
				}
				//If no Allies are found, Attack Alone
				else if (i == EnemyListRef.size() - 1)
				{
					MoveAndAttack(dt);
				}
			}
			else
			{
				// If there is an enemy within the range of 10 meters
				if ((this->pos - EnemyListRef[i].pos).Length() <= 10)
				{
					if (this->pos.x > EnemyListRef[i].pos.x)
					{
						this->pos.x -= 10.f * dt;
					}
					if (this->pos.x < EnemyListRef[i].pos.x)
					{
						this->pos.x += 10.f * dt;
					}
				}
				//If no Allies are found, Attack Alone
				else if (i == EnemyListRef.size() - 1)
				{
					MoveAndAttack(dt);
				}
			}
		}
	}
}

bool Enemy::HasAllies()
{
	for (int i = 0; i < EnemyListRef.size(); i++)
	{
		// Check to see if it's not registering itself
		if (this->pos != EnemyListRef[i].pos)
		{
			// Checks for a length of 5 regardless of Enemy Type
			if ((this->pos - EnemyListRef[i].pos).Length() <= 5)
			{
				return true;
			}
		}
	}

	return false;
}