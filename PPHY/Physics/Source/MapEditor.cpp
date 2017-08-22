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
	alreadyHavePlayer = false;
	Math::InitRNG();

	m_objectCount = 0;
	m_ghost = new GameObject();
	m_ghost->scale.Set(8, 8, 8);
	worldX = worldY = 1;
	//m_playerFace = 

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

void MapEditor::renderText()
{
	std::ostringstream ss;
	if (!start)
	{
		ss << "Choose a level to edit" << std::endl;
		//RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, m_worldHeight * 0.5f);
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 8, 40);
		std::ostringstream ss1;
		ss1 << "Level1";
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss1.str(), Color(0, 1, 0), 3, 28, 36);

		std::ostringstream ss2;
		ss2 << "Level2";
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss2.str(), Color(0, 1, 0), 3, 28, 32);

		std::ostringstream ss3;
		ss3 << "Level3";
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss3.str(), Color(0, 1, 0), 3, 28, 28);

		std::ostringstream ss4;
		ss4 << "Level4";
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss4.str(), Color(0, 1, 0), 3, 28, 24);
	}

	if (start)
	{
		if (saveSuccesfull)
		{
			ss << "Saved Succesfully";
			RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 12);
		}
		if (alreadyHavePlayer)
		{
			ss.str(std::string());
			ss.precision(1);
			ss << "You already have a player on screen";
			RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 2.3f, 0, 3);
		}

		ss.str(std::string());
		ss.precision(5);
		ss <<"Press 'SPACE' to save";
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 5, 50);

	}
}

void MapEditor::mouseControls()
{
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

		if (go->type >= (GameObject::GAMEOBJECT_TYPE)11 && go->type <= (GameObject::GAMEOBJECT_TYPE)18)
			go->pos.Set(posX, posY, 1);
		else if (go->type == GameObject::GO_PLAYER && !alreadyHavePlayer)
		{
			go->pos.Set(posX, posY, 1);
			alreadyHavePlayer = true;
		}
		if (go->type >= (GameObject::GAMEOBJECT_TYPE)4 && go->type <= (GameObject::GAMEOBJECT_TYPE)9)
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
				if (go->type == GameObject::GO_PLAYER)
					alreadyHavePlayer = false;
				go->pos.SetZero();
				go->active = false;
			}
		}
	}
}

void MapEditor::cameraControls(double dt)
{
	if (Application::IsKeyPressed('W'))
	{
		camera.position.y += 200 * dt;
		camera.target.y += 200 * dt;
	}

	if (Application::IsKeyPressed('S'))
	{
		camera.position.y -= 200 * dt;
		camera.target.y -= 200 * dt;
	}
	if (Application::IsKeyPressed('A'))
	{
		camera.position.x -= 200 * dt;
		camera.target.x -= 200 * dt;
	}

	if (Application::IsKeyPressed('D'))
	{
		camera.position.x += 200 * dt;
		camera.target.x += 200 * dt;
	}
}

void MapEditor::selectObjectControl()
{
	static bool isQPressed = false;
	static bool isEPressed = false;
	if (Application::IsKeyPressed('Q') && !isQPressed)
	{
		isQPressed = true;
		choice--;
		if (choice == 0)
			choice = 1;
		m_ghost->type = type(choice)->type;
		m_ghost->scale = type(choice)->scale;
	}

	else if (Application::IsKeyPressed('E') && !isEPressed)
	{
		isEPressed = true;
		choice++;
		if (choice == GameObject::GAMEOBJECT_TYPE::GO_TOTAL)
			choice--;
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

void MapEditor::saveControls()
{
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
	go->type = (GameObject::GAMEOBJECT_TYPE)i;

	if (i >= 10 && i <= 18)
		go->scale.Set(5, 5, 1);
	else
		go->scale.Set(8, 8, 1);
	go->dir.Set(0, 1, 0);
	return go;
}

void MapEditor::LoadObjects(vector<string> data)
{
	string temp;
	GameObject *go;

	for (int i = 0; i < data.size(); i++)
	{
		go = FetchGO();

		for (int k = 0; k < 8; k++)
		{
			temp = "";
			int comma = data[i].find(",");
			temp = data[i].substr(0, comma);

			switch (k)
			{
			case 0:
				go->type = (GameObject::GAMEOBJECT_TYPE)atoi(temp.c_str());
				break;
			case 1:
				go->pos.x = stof(temp);
				break;
			case 2:
				go->pos.y = stof(temp);
				break;
			case 3:
				go->pos.z = stof(temp);
				break;
			case 4:
				go->scale.x = stof(temp);
				break;
			case 5:
				go->scale.y = stof(temp);
				break;
			case 6:
				go->dir.x = stof(temp);
				break;
			case 7:
				go->dir.y = stof(temp);
				break;
			}

			if (go->type == GameObject::GO_PLAYER)
				alreadyHavePlayer = true;

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

	file <<"TYPE  "<<"PX " << "PY " << "PZ"<< "SX " << "SY " << "DX " << "DY " << std::endl;
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		temp = "";
		if (go->active && go->type!= GameObject::GO_NONE)
		{
			temp = std::to_string((int)go->type);
			file << temp << "," << go->pos.x << "," << go->pos.y << "," << go->pos.z << ","<< go->scale.x << "," << go->scale.y << "," << go->dir.x << "," << go->dir.y << std::endl;
		}
		std::cout << temp << std::endl;
	}
	saveSuccesfull = true;
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
		mouseControls();
		cameraControls(dt);
		selectObjectControl();
		saveControls();

		for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject *go = (GameObject *)*it;
			if (go->pos == Vector3(0, 0, 0))
				go->active = false;
		}

		if (Application::IsKeyPressed('F'))
		{
			Application::SetScene(1);
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
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("ball"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BLUE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("blue"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_WALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);// normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("tile_1"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_WALL_2:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);// normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("tile_2"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_WALL_3:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);// normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("tile_3"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_WALL_4:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);// normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("tile_4"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PILLAR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("blue"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PLAYER:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("player"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_MELEE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("enemy"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_RANGED:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("enemy"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BOSS_1:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("blue"), false);
		modelStack.PopMatrix();
		break;
	}
}

void MapEditor::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelStack.PushMatrix();
	modelStack.Scale(worldX, worldY, 0);
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	double x, y;
	Application::GetCursorPos(&x, &y);
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();

	float posX = (static_cast<float>(x) / w * m_worldWidth) + camera.position.x;
	float posY = (h - static_cast<float>(y)) / h * m_worldHeight + camera.position.y;

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

	RenderMesh(MeshBuilder::GetInstance()->GetMesh("reference"), false);

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

	modelStack.PopMatrix();
	//On screen text
	renderText();

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
