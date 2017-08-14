#ifndef SCENE_WIN_H
#define SCENE_WIN_H

#include <vector>
#include "SceneBase.h"

class SceneWin : public SceneBase
{
public:
	SceneWin();
	~SceneWin();

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

