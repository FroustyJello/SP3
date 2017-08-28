#ifndef LOADING_H
#define LOADING_H

#include <vector>
#include "SceneBase.h"

class SceneLoading : public SceneBase
{
public:
	SceneLoading();
	~SceneLoading();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

protected:
	float m_worldWidth;
	float m_worldHeight;

	int clickpos;

	int swit;

	float x1, x2;

	float loadtimer;

	int c_bounceTime;

};

#endif