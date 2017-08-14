#ifndef SCENE_P1WIN_H
#define SCENE_P1WIN_H

#include <vector>
#include "SceneBase.h"

class SceneP1 : public SceneBase
{
public:
	SceneP1();
	~SceneP1();

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
