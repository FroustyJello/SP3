#ifndef SCENE_P2WIN_H
#define SCENE_P2WIN_H

#include <vector>
#include "SceneBase.h"

class SceneP2 : public SceneBase
{
public:
	SceneP2();
	~SceneP2();

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

