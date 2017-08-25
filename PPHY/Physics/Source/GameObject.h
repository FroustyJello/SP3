#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"
#include "SingletonTemplate.h"
#include "MeshBuilder.h"
#include <string>
#include <vector>

struct GameObject :public  Singleton<GameObject>
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		//Walls and static objects
		GO_WALL,
		GO_WALL_2,
		GO_WALL_3,
		GO_WALL_4,
		GO_DOOR,

		//Characters
		GO_PLAYER,
		GO_ENEMY_MELEE,
		GO_ENEMY_MELEE_2,
		GO_ENEMY_RANGED,
		GO_ENEMY_RANGED_2,
		GO_BOSS_1,
		GO_BOSS_2,

		//Miscellaneous
		GO_POWERUP_HEALTH,
		GO_PLAYER_FACE,

		

		//Projectiles
		GO_ARROW,
		GO_FIRE_ARROW,
		GO_ENEMY_BULLET,
		GO_BOSS_LASER,
		GO_BOSS_BULLET_1,
		GO_BOSS_BULLET_2,

		

		GO_TOTAL, //must be last
	};
	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;

	float rotation;
	float momentOfInertia;
	float angularVelocity; //in radians
	float mass;
	float HP;
	float dmg;
	bool active;
	bool hit;
	Vector3 dir; //direction/orientation

	std::vector<GameObject*> AnimList;
	GameObject(GAMEOBJECT_TYPE typeValue = GO_WALL);
	~GameObject();
};

#endif