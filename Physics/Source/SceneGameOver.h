#ifndef SCENE_GAMEOVER_H
#define SCENE_GAMEOVER_H

#include <vector>
#include "SceneBase.h"

class SceneGameOver : public SceneBase
{
public:
	SceneGameOver();
	~SceneGameOver();

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