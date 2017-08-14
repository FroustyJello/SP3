#ifndef SCENE_LOSE_H
#define SCENE_LOSE_H

#include <vector>
#include "SceneBase.h"

class SceneLose : public SceneBase
{
public:
	SceneLose();
	~SceneLose();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

protected:
	float m_worldWidth;
	float m_worldHeight;

	int clickpos;

	int c_bounceTime;

};

#endif
