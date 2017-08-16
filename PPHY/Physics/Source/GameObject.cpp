
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
		this->PlayerRef = PlayerRef;
		for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject *go = (GameObject *)*it;
			if (!go->active)
				continue;
			if (go->type == GameObject::GO_ENEMY_MELEE || go->type == GameObject::GO_ENEMY_RANGED)
			{
				if (this->pos != go->pos)
				{
					if ((this->pos - go->pos).Length() <= 50 && (this->pos - go->pos).Length() > 24)
					{
						if (this->pos.x > go->pos.x)
						{
							this->pos.x -= 12.5f * dt;
						}
						else
						{
							this->pos.x += 12.5f * dt;
						}
					}
				}
			}
			if ((it != m_goList.end()) && (next(it) == m_goList.end()))
			{
				if (this->pos.y - 10 < PlayerRef.y && this->pos.y + 10 > PlayerRef.y)
				{
					if (this->pos.x > PlayerRef.x)
					{
						this->pos.x -= 12.5f * dt;
					}
					else
					{
						this->pos.x += 12.5f * dt;
					}
				}
			}
		}
		break;
	case(GO_ENEMY_RANGED):
		this->PlayerRef = PlayerRef;
		std::cout << PlayerRef << std::endl;
		break;
	case(GO_ARROW):

		break;
	case(GO_ENEMY_BULLET):

		break;
	}
}
