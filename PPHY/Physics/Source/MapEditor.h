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
	GameObject *m_playerFace;

	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	float saveTime;
	float worldX;
	float worldY;

	int m_objectCount;
	int choice;

	bool saveSuccesfull;
	bool start;

	GameObject* type(int i);
	string fileName;
	string selection;
	vector<string>ObjData;

	void renderText();
	void mouseControls();
	void cameraControls(double dt);
	void selectObjectControl();
	void saveControls();
	void chooselevel(int choice);
	void LoadObjects(vector<string> data);
	void SaveFile(vector<GameObject*> List);
};

#endif