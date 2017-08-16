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

public:
	MapEditor();
	~MapEditor();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO();

protected:

	//Physics
	std::vector<GameObject *> m_goList;
	GameObject *m_ghost;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	float saveTime;

	int m_objectCount;
	int choice;
	bool saveSuccesfull;
	bool start;
	void chooselevel(int choice);
	GameObject* type(int i);
	bool preview;
	string fileName;

	vector<string>ObjData;

	void LoadObjects(vector<string> data);
	void SaveFile(vector<GameObject*> List);
};

#endif