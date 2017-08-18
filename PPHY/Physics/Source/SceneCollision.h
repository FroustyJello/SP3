#ifndef SCENE_COLLISION_H
#define SCENE_COLLISION_H

#include "GameObject.h"
#include "Enemy.h"
#include <vector>
#include "SceneBase.h"
#include "csv.h"

using std::vector;

class SceneCollision : public SceneBase
{
	static const int MAX_SPEED = 10;
	static const int BULLET_SPEED = 50;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;

public:
	SceneCollision();
	~SceneCollision();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);
	void RenderEnemy(Enemy *enemy);

	GameObject* FetchGO();
	Enemy* FetchEnemy();

	bool CheckCollision(GameObject *go1, GameObject *go2);
	float CheckCollision2(GameObject *go1, GameObject *go2);
	void CollisionResponse(GameObject *go, GameObject *go2);

	
	GameObject* m_paddle;

protected:

	//Physics
	std::vector<GameObject *> m_goList;
	std::vector<Enemy *> m_enemies;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ghost;
	Enemy *m_enemy;
	GameObject* m_player;
	vector<GameObject*> v_balls;
	int m_objectCount;

	//CPlayer* thePlayerInfo;

	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2;
	Vector3 initialMomentum, finalMomentum;
	float initialKE, finalKE;

	float m_timeEstimated1;
	float m_timeTaken1;
	bool timeStarted;
	int bounce;
	int num_balls;
	int playerMoveIndex;
	float elapesTime;

	vector<string>ObjData;

	Vector3 gravity;

	void LoadObjects(vector<string> data);

	int m_plives;
	int m_elives;

	float ScreenLimit;
};

#endif