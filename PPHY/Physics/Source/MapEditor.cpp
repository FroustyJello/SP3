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
	saveTime = 0;
	CSV reader;
	vector<string> data;

	Math::InitRNG();

	m_objectCount = 0;

	m_ghost = new GameObject(GameObject::GO_BALL);

	data = reader.Load("test.csv", data);

	LoadObjects(data);
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
		GameObject *go = new GameObject(GameObject::GO_BALL);
		m_goList.push_back(go);
	}
	GameObject *go = m_goList.back();
	go->active = true;
	++m_objectCount;
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
				}

				if (temp == "pillar")
				{
					go->type = GameObject::GO_PILLAR;
				}
				if (temp == "ball")
				{
					go->type = GameObject::GO_BALL;
				}
				break;
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
	std::ofstream file;
	string temp;
	file.open("test.csv");
	
	if (file.fail())
		std::cout << "File failed to open" << std::endl;

	file <<"TYPE  "<<"PX " << "PY " << "SX " << "SY " << "DX " << "DY " << std::endl;
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		switch (go->type)
		{
		case GameObject::GO_WALL:
			temp = "wall";
			file << temp << "," << go->pos.x << "," << go->pos.y << "," << go->scale.x << "," << go->scale.y << "," << go->dir.x<<","<<go->dir.y<< std::endl;
			break;
		}
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

		int gridCubeWidth = 5, gridCubeHeight = 5;

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();

		float posX = (static_cast<float>(x) / w * m_worldWidth) + camera.position.x;
		float posY = (h - static_cast<float>(y)) / h * m_worldHeight + camera.position.y;
		
		float cx = floorf(posX / gridCubeWidth) * gridCubeWidth;
		float cy = floorf(posY / gridCubeHeight) * gridCubeHeight;

		GameObject*go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->pos.Set(cx, cy, 0);
		go->dir.Set(0, 1, 0);
		go->scale.Set(5, 5, 0);
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
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
			if ((go->pos - mousepos).Length() < 3)
				go->active = false;
		}
	}
	else if (bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;

		//spawn large GO_BALL
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
				m_objectCount--;
			}
		}
	}

	//Physics Simulation Section
	//dt *= m_speed;

	/*for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
	}*/

	//for (int i = 0; i < m_goList.size() - 1; ++i)
	//{
	//	if (m_goList[i]->pos == m_goList[i + 1]->pos)
	//		m_goList[i + 1]->active = false;
	//}
	
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
	}
}

void MapEditor::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

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
	if (m_ghost->active)
		RenderGO(m_ghost);

	//On screen text
	std::ostringstream ss;
	if (saveSuccesfull)
	{
		ss << "Saved Succesfully";
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 12);
	}
	ss.str(std::string());
	ss.precision(3);
	ss << "Number of Objects: " << m_objectCount;
	RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 9);


	ss.str(std::string());
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 3);
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
