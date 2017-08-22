
#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	mass(1.f),
	momentOfInertia(1.f),
	angularVelocity(0.f),
	dmg(0.f)
{
}

GameObject::~GameObject()
{
}

void GameObject::Update(double dt, Vector3 PlayerRef, std::vector<GameObject *> m_goList)
{

}
