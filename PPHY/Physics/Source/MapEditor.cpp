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
	bounce = 0;

	CSV reader;

	vector<string> data;

	num_balls = 5;
	m_plives = 5;
	m_elives = 5;


	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	m_objectCount = 0;

	m_ghost = new GameObject(GameObject::GO_BALL);

	m_timeEstimated1 = m_timeTaken1 = 0.f;
	bool timeStarted = false; 

	data = reader.Load("test.csv", data);

	LoadObjects(data);


	m_paddle = FetchGO();
	m_paddle->type = GameObject::GO_PADDLE;
	m_paddle->pos.Set(10, 50, 0);
	m_paddle->dir.Set(1, 0, 0);
	m_paddle->scale.Set(2, 10, 1.f);

	m_enemy = FetchGO();
	m_enemy->type = GameObject::GO_WALL;
	m_enemy->pos.Set(m_paddle->pos.x + 120, m_paddle->pos.y, m_paddle->pos.z);
	m_enemy->dir.Set(1, 0, 0);
	m_enemy->scale.Set(2, 10, 1.f);


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

bool MapEditor::CheckCollision(GameObject *go1, GameObject *go2)
{
	if (go2->type == GameObject::GO_BALL || go2->type == GameObject::GO_BALLDYING)
	{
		Vector3 relativePosition = go1->pos - go2->pos;
		Vector3 relativeVelocity = go1->vel - go2->vel;
		float r1 = go1->scale.x;
		float r2 = go2->scale.x;

		return relativeVelocity.Dot(relativePosition) < 0 && relativePosition.LengthSquared() < (r1 + r2) * (r1 + r2);
	}
	else if (go1->type == GameObject::GO_BALL && go2->type == GameObject::GO_WALL)
	{	
		Vector3 w0 = go2->pos;
		Vector3 b1 = go1->pos;
		float r = go1->scale.x;
		float h = go2->scale.x;
		float l = go2->scale.y;
		Vector3 NP = go2->dir.Cross(Vector3(0, 0, 1));
		Vector3 N = go2->dir;

		Vector3 relativePos = b1 - w0;
		if (relativePos.Dot(N) > 0)
			N = -N;


		return go1->vel.Dot(N) > 0 && abs((w0 - b1).Dot(N)) < r + h *0.5f && abs((w0 - b1).Dot(NP)) < r + l *0.5f;
	}

	else if (go1->type == GameObject::GO_BALL && go2->type == GameObject::GO_PADDLE)
	{
		Vector3 w0 = go2->pos;
		Vector3 b1 = go1->pos;
		float r = go1->scale.x;
		float h = go2->scale.x;
		float l = go2->scale.y;
		Vector3 NP = go2->dir.Cross(Vector3(0, 0, 1));
		Vector3 N = go2->dir;

		Vector3 relativePos = b1 - w0;
		if (relativePos.Dot(N) > 0)
			N = -N;


		return go1->vel.Dot(N) > 0 && abs((w0 - b1).Dot(N)) < r + h *0.5f && abs((w0 - b1).Dot(NP)) < r + l *0.5f;
	}

	else if (go1->type == GameObject::GO_BALL && go2->type == GameObject::GO_PILLAR)
	{
		Vector3 p1 = go1->pos;
		Vector3 p2 = go2->pos;
		float r1 = go1->scale.x;
		float r2 = go2->scale.x;
		Vector3 u1 = go1->vel;


		return (p2-p1).LengthSquared() < (r1 + r2) * (r1 + r2) && (go2->pos - go1->pos).Dot(u1) > 0;
	}


	return false;
}

float MapEditor::CheckCollision2(GameObject * go1, GameObject * go2)
{
	Vector3 rel = go1->vel - go2->vel;
	Vector3 dir = go1->pos - go2->pos;
	float r = go1->scale.x + go2->scale.x;

	if (rel.Dot(dir) > 0)
		return -1;
	float a = rel.LengthSquared();
	float b = 2.f*rel.Dot(dir);
	float c = dir.LengthSquared() - (r*r);

	float d = b*b - 4 * a*c;
	if (d < 0)
		return -1;

	float t = (-b - sqrt(d)) / (2 * a);
	if (t < 0)
		t = (-b + sqrt(d)) / (2 * a);
	return t;
}

void MapEditor::CollisionResponse(GameObject * go, GameObject * go2)
{
	if (go2->type == GameObject::GO_BALL || go2->type == GameObject::GO_BALLDYING)
	{
		Vector3 u1 = go->vel;
		Vector3 u2 = go2->vel;
		Vector3 N = (go2->pos - go->pos).Normalize();
		Vector3 u1N = u1.Dot(N) * N;
		Vector3 u2N = u2.Dot(N) * N;

		go->vel = u1 + 2 * m2 / (m1 + m2) * (u2N - u1N);
		go2->vel = u2 + 2 * m1 / (m1 + m2) * (u1N - u2N);
	}
	else if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_PADDLE)
	{
		Vector3 u = go->vel;
		Vector3 N = go2->dir;

		go->vel = u - (2 * u.Dot(N) * N);

		if (go2->type == GameObject::GO_PADDLE)
		{
			go->vel *= 1.4;
		}
	}
	else if (go2->type == GameObject::GO_PILLAR)
	{
		Vector3 u = go->vel;
		Vector3 N = (go2->pos - go->pos).Normalize();

		go->vel = u - 2 * u.Dot(N) * N;
	}

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

	file << "PX " << "PY " << "SX " << "SY " << "DX " << "DY " << std::endl;
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
	std::cout << "Save Successful" << std::endl;
	file.close();
}

void MapEditor::Update(double dt)
{
	SceneBase::Update(dt);
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();


	if (Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if (Application::IsKeyPressed('0'))
	{
		for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject *go = (GameObject *)*it;

			if (go->active)
				go->active = false;
		}
	}

	if (Application::IsKeyPressed('W') && m_paddle->pos.y < 66)
	{
		//m_paddle->pos.y += 25 * dt * m_speed;
		camera.position.x += 50 * dt;
		camera.target.x += 50 * dt;
	}

	if (Application::IsKeyPressed('S') && m_paddle->pos.y > 35)
	{
		//m_paddle->pos.y -= 25 * dt * m_speed;
		camera.position.x -= 50 * dt;
		camera.target.x -= 50 * dt;
	}


	if (Application::IsKeyPressed('O'))
	{
		
	}

	static bool bSpaceState = false;
	if (!bSpaceState && Application::IsKeyPressed(VK_SPACE))
	{
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

		//float offsetx = (0.5f * m_worldWidth) - camera.target.x;

		float posX = (static_cast<float>(x) / w * m_worldWidth) + camera.position.x;
		float posY = (h - static_cast<float>(y)) / h * m_worldHeight + camera.position.y;
		


		float cx = floorf(posX / gridCubeWidth) * gridCubeWidth;
		float cy = floorf(posY / gridCubeHeight) * gridCubeHeight;

		GameObject*go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->pos.Set(cx, cy, 0);
		go->dir.Set(0, 1, 0);
		go->scale.Set(5, 5, 1);
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
	}

	static bool bRButtonState = false;
	if (!bRButtonState && Application::IsMousePressed(1))
	{
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
				go->active = false;
		}

		/*GameObject *go = FetchGO();
		go->type = GameObject::GO_CUBE;
		go->pos = m_ghost->pos;
		go->scale.Set(m_ghost->pos.x - posX, m_ghost->pos.y - posY, 0);
		m_ghost->active = false;*/
	}

	//Physics Simulation Section
	dt *= m_speed;

		for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject *go = (GameObject *)*it;
			if (!go->active)
				continue;
	
			for (std::vector<GameObject *>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
			{
				GameObject *go2 = static_cast<GameObject *>(*it2);

				if(go->active && go2->active)
				if (go2->type == GameObject::GO_CUBE)
				{
					float distSq = (go2->pos - go->pos).Length();
				}

				if (!go2->active)
					continue;
				//Exercise 1: move collision code to CheckCollision() -OK!
				GameObject *goA = go, *goB = go2;
				//Practical 4, Exercise 13: improve collision detection algorithm
				if (go->type != GameObject::GO_BALL)
				{
					if (go2->type != GameObject::GO_BALL)
						continue;
					goA = go2;
					goB = go;
				}
	
				if (CheckCollision(goA, goB))
				{
					m1 = go->mass;
					m2 = go2->mass;
					u1 = go->vel;
					u2 = go2->vel;
	
					CollisionResponse(goA, goB);
	
					v1 = go->vel;
					v2 = go2->vel;
					go->vel = 0.95 * go->vel;
					go2->vel = 0.95 * go2->vel;
	
				}
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
	case GameObject::GO_BALLDYING:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALLDYING], false);
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
	case GameObject::GO_PADDLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, m_paddle->pos.y, go->pos.z);
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

	case GameObject::GO_CUBE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);// normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
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
	ss << "Player lives: " << m_plives;
	RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 12);

	ss.str(std::string());
	ss.precision(3);
	ss << "Enemy lives: " << m_elives;
	RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 9);

	//ss.str(std::string());
	//ss.precision(5);
	//ss << "Initial momentum: " << initialMomentum;
	//RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 15);

	//ss.str(std::string());
	//ss.precision(5);
	//ss << "Final   momentum: " << finalMomentum;
	//RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 12);

	//Exercise 3: render initial and final kinetic energy

	ss.str(std::string());
	ss.precision(3);
	ss << "Num_Balls: " << num_balls;
	RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 6);

	ss.str(std::string());
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 1, 0), 3, 0, 3);

	//RenderTextOnScreen(meshList[GEO_CALIBRI], "Collision", Color(0, 1, 0), 3, 0, 0);
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
