
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

void GameObject::Update(double dt, Vector3 PlayerRef)
{
	// Behaviour
	switch (this->type)
	{
	case(GO_ENEMY_MELEE):
		this->PlayerRef = PlayerRef;
		std::cout << PlayerRef << std::endl;
		break;
	case(GO_ENEMY_RANGED):

		break;
	case(GO_ARROW):

		break;
	case(GO_ENEMY_BULLET):

		break;
	}
}
