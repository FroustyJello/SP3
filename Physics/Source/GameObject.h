#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_CUBE,
		GO_BLUE,

		GO_ASTEROID_BIG,	 //asteroid
		GO_ASTEROID_SMALL,	 //asteroid
		GO_PLANET,
		GO_SHIP,		 //player ship
		GO_BULLET,		 //player bullet
		GO_ENEMY,		 //enemy ship
		GO_MISSILE,		 //player missile
		GO_POWERUP_HEALTH,		 //powerup item

		GO_WALL,
		GO_PADDLE,
		GO_PILLAR,

		GO_TOTAL, //must be last
	};
	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	float rotation;
	bool active;
	float mass;

	Vector3 dir; //direction/orientation
	float momentOfInertia;
	float angularVelocity; //in radians

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif