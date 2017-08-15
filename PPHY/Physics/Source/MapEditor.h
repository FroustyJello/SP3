#ifndef MAP_EDITOR_H
#define MAP_EDITOR_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"
#include "csv.h"
#include <fstream>
#include<ostream>
#include <math.h>
	
using std::vector;

class MapEditor : public SceneBase
{
	static const int MAX_SPEED = 10;
	static const int BULLET_SPEED = 50;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;

public:
	MapEditor();
	~MapEditor();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO();

	bool CheckCollision(GameObject *go1, GameObject *go2);
	float CheckCollision2(GameObject *go1, GameObject *go2);
	void CollisionResponse(GameObject *go, GameObject *go2);

protected:

	//Physics
	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ghost;
	GameObject* m_paddle;
	GameObject* m_enemy;
	vector<GameObject*> v_balls;
	int m_objectCount;

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

	vector<string>ObjData;

	void LoadObjects(vector<string> data);
	void SaveFile(vector<GameObject*> List);

	int m_plives;
	int m_elives;
};

#endif