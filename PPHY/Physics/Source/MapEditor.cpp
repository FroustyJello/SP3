#include "MapEditor.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

MapEditor::MapEditor()
{
}

MapEditor::~MapEditor()
{
}

void MapEditor::Init()
{
	SceneBase::Init();
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	saveSuccesfull = false;
	start = false;
	saveTime = 0;
	fileName = "";
	choice = 0;
	selection = "Wall1";

	Math::InitRNG();

	m_objectCount = 0;
	m_ghost = new GameObject(GameObject::GO_WALL);
	m_ghost->scale.Set(8, 8, 8);

}

GameObject* MapEditor::FetchGO()
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->active)
		{
			go->active = true;
			++m_objectCount;
			return go;
		}
	}
	for (unsigned i = 0; i < 10; ++i)
	{
		GameObject *go = new GameObject();
		m_goList.push_back(go);
	}
	GameObject *go = m_goList.back();
	go->active = true;
	++m_objectCount;
	return go;
}


void MapEditor::chooselevel(int choice)
{
	CSV reader;
	vector<string> data;
	switch (choice)
	{
	case 1: 
		fileName = "level1.csv";
		break;
	case 2:
		fileName = "level2.csv";
		break;
	case 3:
		fileName = "level3.csv";
		break;
	case 4:
		fileName = "level4.csv";
		break;
	}
	data = reader.Load(fileName, data);
	LoadObjects(data);
	start = true;
	return;
}

GameObject * MapEditor::type(int i)
{
	GameObject *go = FetchGO();
	switch (i)
	{
	case 0:
		go->type = GameObject::GO_WALL;
		go->scale.Set(8, 8, 8);
		go->dir.Set(0, 1, 0);
		selection = "Wall1";
		break;
	case 1:
		go->type = GameObject::GO_PILLAR;
		go->scale.Set(8, 8, 8);
		go->dir.Set(0, 1, 0);
		selection = "Pillar";
		break;
	case 2:
		go->type = GameObject::GO_PLAYER;
		go->scale.Set(5, 5, 5);
		go->dir.Set(0, 1, 0);
		selection = "Player";
		break;
	}
	return go;
}

void MapEditor::LoadObjects(vector<string> data)
{
	string temp;
	GameObject *go;

	for (int i = 0; i < data.size(); i++)
	{
		go = FetchGO();

		for (int k = 0; k < 7; k++)
		{
			temp = "";
			int comma = data[i].find(",");
			temp = data[i].substr(0, comma);

			switch (k)
			{
			case 0:
				if (temp == "wall")
				{
					go->type = GameObject::GO_WALL;
					break;
				}

				if (temp == "pillar")
				{
					go->type = GameObject::GO_PILLAR;
					break;
				}

				if (temp == "player")
				{
					go->type = GameObject::GO_PLAYER;
					break;
				}
				
			case 1:
				go->pos.x = stof(temp);
				break;
			case 2:
				go->pos.y = stof(temp);
				break;
			case 3:
				go->scale.x = stof(temp);
				break;
			case 4:
				go->scale.y = stof(temp);
				break;
			case 5:
				go->dir.x = stof(temp);
				break;
			case 6:
				go->dir.y = stof(temp);
				break;
			}

			if (go->dir != Vector3{ 0,0,0 })
			{
				go->dir.Normalize();
			}
			data[i].erase(0, comma + 1);
		}
	}
}

void MapEditor::SaveFile(vector<GameObject*> List)
{
	if (!m_goList.empty())
	{
		for (int i = 0; i < m_goList.size() - 1; ++i)
		{
			if (m_goList[i]->pos == m_goList[i + 1]->pos)
				m_goList[i]->active = false;
		}
	}

	std::ofstream file;
	string temp;
	file.open(fileName);
	
	if (file.fail())
		std::cout << "File failed to open" << std::endl;

	file <<"TYPE  "<<"PX " << "PY " << "SX " << "SY " << "DX " << "DY " << std::endl;
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			switch (go->type)
			{
			case GameObject::GO_WALL:
				temp = "wall";
				break;
			case GameObject::GO_PILLAR:
				temp = "pillar";
				break;
			case GameObject::GO_PLAYER:
				temp = "player";
				break;
			}
		file << temp << "," << go->pos.x << "," << go->pos.y << "," << go->scale.x << "," << go->scale.y << "," << go->dir.x << "," << go->dir.y << std::endl;
		}
		std::cout << temp << std::endl;
	}
	saveSuccesfull = true;
	//std::cout << "Save Successful" << std::endl;
	file.close();
}

void MapEditor::Update(double dt)
{
	SceneBase::Update(dt);
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	if (saveSuccesfull)
	{
		saveTime += 1 * dt;
		if (saveTime > 3)
		{
			saveSuccesfull = false;
			saveTime = 0;
		}
	}
	if (!start)
	{
		if (Application::IsKeyPressed('1'))
			chooselevel(1);
		if (Application::IsKeyPressed('2'))
			chooselevel(2);
		if (Application::IsKeyPressed('3'))
			chooselevel(3);
		if (Application::IsKeyPressed('4'))
			chooselevel(4);
	}

	if (start)
	{
		if (Application::IsKeyPressed('0'))
		{
			for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject *go = (GameObject *)*it;

				if (go->active)
					go->active = false;
			}
			m_objectCount = 0;
		}

		if (Application::IsKeyPressed('W'))
		{
			camera.position.y += 100 * dt;
			camera.target.y += 100 * dt;
		}

		if (Application::IsKeyPressed('S'))
		{
			camera.position.y -= 100 * dt;
			camera.target.y -= 100 * dt;
		}
		if (Application::IsKeyPressed('A'))
		{
			camera.position.x -= 100 * dt;
			camera.target.x -= 100 * dt;
		}

		if (Application::IsKeyPressed('D'))
		{
			camera.position.x += 100 * dt;
			camera.target.x += 100 * dt;
		}

		static bool bSpaceState = false;
		if (!bSpaceState && Application::IsKeyPressed(VK_SPACE))
		{
			bSpaceState = true;
			SaveFile(m_goList);
		}
		else if (bSpaceState && !Application::IsKeyPressed(VK_SPACE))
		{
			bSpaceState = false;
		}
		//Mouse Section
		static bool bLButtonState = false;
		if (!bLButtonState && Application::IsMousePressed(0))
		{
			bLButtonState = true;
			std::cout << "LBUTTON DOWN" << std::endl;

			int gridCubeWidth = 8, gridCubeHeight = 8;

			double x, y;
			Application::GetCursorPos(&x, &y);
			int w = Application::GetWindowWidth();
			int h = Application::GetWindowHeight();

			float posX = (static_cast<float>(x) / w * m_worldWidth) + camera.position.x;
			float posY = (h - static_cast<float>(y)) / h * m_worldHeight + camera.position.y;

			float cx = floorf(posX / gridCubeWidth) * gridCubeWidth;
			float cy = floorf(posY / gridCubeHeight) * gridCubeHeight;

			m_ghost->active = false;
			GameObject*go = type(choice);
			go->pos.Set(cx, cy, 0);
		}
		else if (bLButtonState && !Application::IsMousePressed(0))
		{
			m_ghost->active = true;
			bLButtonState = false;
			std::cout << "LBUTTON UP" << std::endl;
		}

		static bool bRButtonState = false;
		if (!bRButtonState && Application::IsMousePressed(1))
		{
			double x, y;
			Application::GetCursorPos(&x, &y);
			int w = Application::GetWindowWidth();
			int h = Application::GetWindowHeight();
			float posX = static_cast<float>(x) / w * m_worldWidth + camera.position.x;
			float posY = (h - static_cast<float>(y)) / h * m_worldHeight + camera.position.y;

			Vector3 mousepos = Vector3(posX, posY, 0);

			for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject *go = (GameObject *)*it;
				if ((go->pos - mousepos).Length() < 5)
				{
					 go->active = false;
					 delete go;
				}
			}
		}
		else if (bRButtonState && !Application::IsMousePressed(1))
		{
			/*bRButtonState = false;
			std::cout << "RBUTTON UP" << std::endl;

			double x, y;
			Application::GetCursorPos(&x, &y);
			int w = Application::GetWindowWidth();
			int h = Application::GetWindowHeight();
			float posX = static_cast<float>(x) / w * m_worldWidth + camera.position.x;
			float posY = (h - static_cast<float>(y)) / h * m_worldHeight + camera.position.y;

			Vector3 mousepos = Vector3(posX, posY, 0);


			for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject *go = (GameObject *)*it;
				if ((go->pos - mousepos).Length() < 3)
				{
					go->active = false;
					delete go;
					m_objectCount--;
				}
			}*/
		}
		static bool isQPressed = false;
		static bool isEPressed = false;
		if (Application::IsKeyPressed('Q') && !isQPressed)
		{
			isQPressed = true;
			choice--;
			m_ghost->type = type(choice)->type;
			m_ghost->scale = type(choice)->scale;
		}

		else if (Application::IsKeyPressed('E') && !isEPressed)
		{
			isEPressed = true;
			choice++;
			m_ghost->type = type(choice)->type;
			m_ghost->scale = type(choice)->scale;
		}
		 if (isQPressed && !Application::IsKeyPressed('Q'))
		{
			isQPressed = false;
			m_ghost->active = false;
		}
		else if (isEPressed && !Application::IsKeyPressed('E'))
		{
			isEPressed = false;
			m_ghost->active = false;
		}
	}
}

void MapEditor::RenderGO(GameObject *go)
{
	switch (go->type)
	{
	case GameObject::GO_BALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BLUE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BLUE], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_WALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);// normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PILLAR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BLUE], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PLAYER:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[PLAYER], false);
		modelStack.PopMatrix();
		break;
	}
}

void MapEditor::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();



	//int gridCubeWidth = 8, gridCubeHeight = 8;

	double x, y;
	Application::GetCursorPos(&x, &y);
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();

	float posX = (static_cast<float>(x) / w * m_worldWidth) + camera.position.x;
	float posY = (h - static_cast<float>(y)) / h * m_worldHeight + camera.position.y;

	/*float cx = floorf(posX / gridCubeWidth) * gridCubeWidth;
	float cy = floorf(posY / gridCubeHeight) * gridCubeHeight;*/

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);

	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
	);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	RenderMesh(meshList[GEO_AXES], false);

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}

	modelStack.PushMatrix();
	modelStack.Translate(posX, posY, 0);
	RenderGO(m_ghost);
	modelStack.PopMatrix();

	//On screen text
	std::ostringstream ss;


	if (!start)
	{
		ss << "Choose a level to edit" << std::endl;
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, m_worldHeight * 0.5f);

		std::ostringstream ss1;
		ss1 << "Level1";
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss1.str(), Color(0, 1, 0), 3, 25, 40);

		std::ostringstream ss2;
		ss2 << "Level2";
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss2.str(), Color(0, 1, 0), 3, 25, 30);

		std::ostringstream ss3;
		ss3 << "Level3";
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss3.str(), Color(0, 1, 0), 3, 25, 20);

		std::ostringstream ss4;
		ss4 << "Level4";
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss4.str(), Color(0, 1, 0), 3, 25, 10);
	}

	if (start)
	{
		if (saveSuccesfull)
		{
			ss << "Saved Succesfully";
			RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 12);
		}
		ss.str(std::string());
		ss.precision(5);
		ss << "Current Object: " << selection;
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 3);
	}
}

void MapEditor::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}

	if (m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}
