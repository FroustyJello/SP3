
#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	mass(1.f),
	momentOfInertia(1.f),
	angularVelocity(0.f),
	dmg(0.f),
	HP(1.f),
	hit(true)
{
}

GameObject::~GameObject()
{
}

void GameObject::Update(double dt)
{
	if (!this->active)
	{
		switch (type)
		{
		case(GO_ARROW):

			break;
		case(GO_FIRE_ARROW):
			break;
		case(GO_ENEMY_BULLET):
			break;
		}
	}
}