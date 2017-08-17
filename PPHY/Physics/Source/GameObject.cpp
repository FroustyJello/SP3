
#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	mass(1.f),
	momentOfInertia(1.f),
	angularVelocity(0.f)
{
}

GameObject::~GameObject()
{
}

void GameObject::Update(double dt, Vector3 PlayerRef, std::vector<GameObject *> m_goList)
{
	// Behaviour
	switch (this->type)
	{
	case(GO_ENEMY_MELEE):
	{
		// Variable Update
		attackBT--;
		this->PlayerRef = PlayerRef;

		// Continous changing/checking of which gameobject is the closest ally
		for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject *go = (GameObject *)*it;
			if (!go->active)
				continue;
			if (go->type == GameObject::GO_ENEMY_MELEE || go->type == GameObject::GO_ENEMY_RANGED)
			{
				if (this->pos != go->pos)
				{
					if (ClosestEnemy == NULL)
					{
						std::cout << "Assign" << std::endl;
						ClosestEnemy = go;
					}
					else if ((this->pos - go->pos).Length() < (this->pos - ClosestEnemy->pos).Length())
					{
						std::cout << "Change!" << std::endl;
						ClosestEnemy = go;
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
						this->pos.x += 12.5f * dt;
					else
						this->pos.x -= 12.5f * dt;
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
		break;
	case(GO_ENEMY_RANGED):
	{
		// Variable Update
		attackBT--;
		this->PlayerRef = PlayerRef;

		// Continous changing/checking of which gameobject is the closest ally
		for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject *go = (GameObject *)*it;
			if (!go->active)
				continue;
			if (go->type == GameObject::GO_ENEMY_MELEE || go->type == GameObject::GO_ENEMY_RANGED)
			{
				if (this->pos != go->pos)
				{
					if (ClosestEnemy == NULL)
					{
						std::cout << "Assign" << std::endl;
						ClosestEnemy = go;
					}
					else if ((this->pos - go->pos).Length() < (this->pos - ClosestEnemy->pos).Length())
					{
						std::cout << "Change!" << std::endl;
						ClosestEnemy = go;
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
		break;
	case(GO_BOSS_1):
		break;
	case(GO_ARROW):
		// Reverse Apple throwing Collision Effect
		break;
	case(GO_ENEMY_BULLET):
		// Bounce off
		break;
	case(GO_BOSS_LASER):
		// Remove immediately, no physics required
		break;
	}
}
