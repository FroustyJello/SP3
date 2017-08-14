#ifndef SCENE_HELP_H
#define SCENE_HELP_H

#include <vector>
#include "SceneBase.h"

class SceneHelp : public SceneBase
{
public:
	SceneHelp();
	~SceneHelp();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

protected:
	float m_worldWidth;
	float m_worldHeight;
};

#endif