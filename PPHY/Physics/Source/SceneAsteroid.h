#ifndef SCENE_ASTEROID_H
#define SCENE_ASTEROID_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneAsteroid : public SceneBase
{
	static const int MAX_SPEED = 20;
	static const int BULLET_SPEED = MAX_SPEED * 3;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;
	static const int PI = 3.14159265359;

public:
	SceneAsteroid();
	~SceneAsteroid();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	void SpawnAsteroids(unsigned number);
	void SpawnEnemies(unsigned enemy);
	void SpawnMissle(unsigned enemy);
	void SpawnPowerup(Vector3 go);
	void RandomSpawnPowerup(Vector3 go);

	void RenderSpace();

	GameObject* FetchGO();
protected:

	//Physics
	std::vector<GameObject *> m_goList;
	std::vector<GameObject *> m_goEnemy;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ship;
	GameObject *p_planet;
	Vector3 m_force;

	int m_objectCount;
	int m_lives;
	int m_score;

	float m_currentTime;
	float m_bounceTime;

	Vector3 m_torque;	float m_shipAngle;

	//Enemy param
	int e_spawnTime;

	//Plane param
	int p_lives;

	//waves
	bool start;
	int s_timer;

	bool w_wave1;
	bool d_wave1;
	bool w_clear1;
	bool d_clear1;
	int w_time1;

	bool w_wave2;
	bool d_wave2;
	bool w_clear2;
	bool d_clear2;
	int w_time2;

	bool w_wave3;
	bool d_wave3;
	bool w_clear3;
	bool d_clear3;
	int w_time3;

	//pause
	bool paused;
	int clickpos;
	int c_bounceTime;

	bool game_Victory;

};

#endif