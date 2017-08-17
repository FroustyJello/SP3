#include "SceneCollision.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneCollision::SceneCollision()
{
}

SceneCollision::~SceneCollision()
{
}

void SceneCollision::Init()
{
	ScreenLimit = 60.f;
	SceneBase::Init();
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	bounce = 0;

	gravity.Set(0, -50.0f, 0);
	playerMoveIndex = 0;
	elapesTime = 0;
	CSV reader;

	vector<string> data;

	num_balls = 5000;
	m_plives = 5;
	m_elives = 5;

	
	thePlayerInfo = CPlayer::GetInstance();
	thePlayerInfo->Init();
	/*thePlayerInfo->SetPos(Vector3(10, 50, 0));*/
	//m_player = thePlayerInfo;
	//m_player = FetchGO();
	thePlayerInfo->type = GameObject::GO_PLAYER;
	thePlayerInfo->scale.Set(4, 4, 4);
	thePlayerInfo->active = true;
	//thePlayerInfo->pos.Set(10, 50, 0);
	thePlayerInfo->dir.Set(1, 0, 0);
	/*thePlayerInfo = thePlayerInfo;*/
	//m_goList.push_back(thePlayerInfo);

	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	m_objectCount = 0;

	//m_ghost = new GameObject(GameObject::GO_BALL);

	m_timeEstimated1 = m_timeTaken1 = 0.f;
	bool timeStarted = false;

	data = reader.Load("level1.csv", data);

	LoadObjects(data);

	for (int i = 0; i < m_goList.size(); ++i)
	{
		if (m_goList[i]->type == GameObject::GO_PLAYER)
		{
			thePlayerInfo->pos = m_goList[i]->pos;
			m_goList[i] = thePlayerInfo;
		}
	}


	GameObject *go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->pos.Set(10, 10, 10);
	go->dir.Set(0, 1, 0);
	go->scale.Set(1, 100, 1);



	/*m_paddle = FetchGO();
	m_paddle->type = GameObject::GO_BALL;
	m_paddle->pos.Set(10, 50, 0);
	m_paddle->dir.Set(1, 0, 0);
	m_paddle->scale.Set(5, 5, 1.f);*/

	/*m_enemy = FetchGO();
	m_enemy->type = GameObject::GO_ENEMY_MELEE;
	m_enemy->pos.Set(m_paddle->pos.x + 120, m_paddle->pos.y - 20, m_paddle->pos.z);
	m_enemy->dir.Set(1, 0, 0);
	m_enemy->scale.Set(5, 5, 1.f);*/
}

GameObject* SceneCollision::FetchGO()
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

bool SceneCollision::CheckCollision(GameObject *go1, GameObject *go2)
{
	if (go2->type == GameObject::GO_BALL)
	{
		Vector3 relativePosition = go1->pos - go2->pos;
		Vector3 relativeVelocity = go1->vel - go2->vel;
		float r1 = go1->scale.x;
		float r2 = go2->scale.x;

		return relativeVelocity.Dot(relativePosition) < 0 && relativePosition.LengthSquared() < (r1 + r2) * (r1 + r2);
	}
	else if ((go1->type == GameObject::GO_PLAYER || go1->type == GameObject::GO_BALL) && go2->type == GameObject::GO_WALL)
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

	else if (go1->type == GameObject::GO_BALL && go2->type == GameObject::GO_WALL)
	{
		Vector3 w0 = go2->pos;
		Vector3 b1 = go1->pos;
		float r = go1->scale.x;
		float h = go2->scale.x;
		float l = go2->scale.y;
		Vector3 NP = go2->dir.Cross(Vector3(0, 0, 1));
		Vector3 N = go2->dir;

		std::cout << "COLLIDED BALL WALL" << std::endl;

		Vector3 relativePos = b1 - w0;
		if (relativePos.Dot(N) > 0)
			N = -N;

		//New box code
		Vector3 detect(Math::Clamp((b1 - w0).x, 0.f, h / 2), Math::Clamp((b1 - w0).y, 0.f, l / 2), 0);
		detect += w0;


		if ((detect - b1).Length() < r)
			return go1->vel.Dot(N) > 0 && abs((w0 - b1).Dot(N)) < r + h *0.5f && abs((w0 - b1).Dot(NP)) < r + l *0.5f;
			//return true;
	}

	else if (go1->type == GameObject::GO_BALL && go2->type == GameObject::GO_PILLAR)
	{
		Vector3 p1 = go1->pos;
		Vector3 p2 = go2->pos;
		float r1 = go1->scale.x;
		float r2 = go2->scale.x;
		Vector3 u1 = go1->vel;


		return (p2 - p1).LengthSquared() < (r1 + r2) * (r1 + r2) && (go2->pos - go1->pos).Dot(u1) > 0;
	}


	return false;
}

float SceneCollision::CheckCollision2(GameObject * go1, GameObject * go2)
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

void SceneCollision::CollisionResponse(GameObject * go, GameObject * go2)
{

	if (go2->type == GameObject::GO_BALL)
	{
		Vector3 u1 = go->vel;
		Vector3 u2 = go2->vel;
		Vector3 N = (go2->pos - go->pos).Normalize();
		Vector3 u1N = u1.Dot(N) * N;
		Vector3 u2N = u2.Dot(N) * N;

		std::cout << "COLLIDED BALL" << std::endl;

		go->vel = u1 + 2 * m2 / (m1 + m2) * (u2N - u1N);
		go2->vel = u2 + 2 * m1 / (m1 + m2) * (u1N - u2N);
	}
	else if (go2->type == GameObject::GO_WALL && go->type == GameObject::GO_PLAYER)
	{
		Vector3 u = go->vel;
		Vector3 N = go2->dir;
		go->vel = u - (2 * u.Dot(N) * N);

		float limit = 0;

		limit = go->vel.LengthSquared();

		if (go->vel.LengthSquared() <= limit)
		{
			
			go->vel = 0;
		}
	}

	else if (go2->type == GameObject::GO_WALL && go->type == GameObject::GO_BALL)
	{
		//Vector3 u = go->vel;
		//Vector3 N = go2->dir;
		//go->vel = u - (2 * u.Dot(N) * N);

		Vector3 u = go->vel;
		Vector3 N = go2->dir;
		
		Vector3 distance;
		distance = go->pos - go2->pos;
		distance.x = Math::Clamp(distance.x, -go2->scale.x / 2, go2->scale.x / 2);
		distance.y = Math::Clamp(distance.y, -go2->scale.y / 2, go2->scale.y / 2);

		distance += N;

		Vector3 right = N.Cross(Vector3(0, 0, 1));

		if (distance.Dot(right) > 0)
		{
			N = N.Cross(Vector3(0, 0, 1));
			N = -N;
		}

		if (distance.Dot(right) < 0)
		{
			N = N.Cross(Vector3(0, 0, 1));
		}

		go->vel = u - (2 * u.Dot(N) * N);




	}

	else if (go2->type == GameObject::GO_PILLAR)
	{
		Vector3 u = go->vel;
		Vector3 N = (go2->pos - go->pos).Normalize();

		go->vel = u - 2 * u.Dot(N) * N;
	}

}

void SceneCollision::LoadObjects(vector<string> data)
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
				if (temp == "player")
				{
					go->type = GameObject::GO_PLAYER;
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

void SceneCollision::Update(double dt)
{
	SceneBase::Update(dt);
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//if (m_paddle->pos.x > ScreenLimit)
	//{
	//	ScreenLimit = m_paddle->pos.x + 1;
	//	camera.position.x += 25 * dt * m_speed;
	//	camera.target.x += 25 * dt * m_speed;
	//
	//	std::cout << floor(m_paddle->pos.x) << " > " << ScreenLimit << std::endl;
	//}
	thePlayerInfo->Update(dt);



	if (Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if (Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}

	if (Application::IsKeyPressed('W'))
	{
	}

	if (Application::IsKeyPressed('S'))
	{
	}

	if (Application::IsKeyPressed('D'))
	{
		//Application::GetWindowWidth() * 0.75f
		if (thePlayerInfo->position.x > ScreenLimit)
		{
			camera.position.x += 25 * dt * m_speed;
			camera.target.x += 25 * dt * m_speed;
			ScreenLimit += 25 * dt * m_speed;
		}
	}

	if (Application::IsKeyPressed('A'))
	{
		thePlayerInfo->position.x -= 25 * dt * m_speed;
		//Application::GetWindowWidth() * 0.25f
		if (thePlayerInfo->position.x < ScreenLimit)
		{
			camera.position.x -= 25 * dt * m_speed;
			camera.target.x -= 25 * dt * m_speed;
			ScreenLimit -= 25 * dt * m_speed;
		}
	}

	bounce++;

	if (Application::IsKeyPressed('O') && bounce > 20 && num_balls != 0)
	{
		GameObject *go = FetchGO();
		go->active = true;
		go->type = GameObject::GO_BALL;

		go->pos.Set(thePlayerInfo->pos.x + 5, thePlayerInfo->pos.y + 5, thePlayerInfo->pos.z);
		go->vel.Set(200, Math::RandFloatMinMax(-10, 10), 0);
		float sc = 2;
		go->scale.Set(sc, sc, sc);
		go->mass = sc * sc * sc;
		v_balls.push_back(go);
		bounce = 0;
		num_balls--;
	}

	if (Application::IsKeyPressed(VK_RIGHT))
	{
		camera.position.x += 10 * dt;
		camera.target.x += 10 * dt;
	}

	if (Application::IsKeyPressed(VK_LEFT))
	{
		camera.position.x -= 10 * dt;
		camera.target.x -= 10 * dt;
	}

	if (Application::IsKeyPressed(VK_UP))
	{
		camera.position.y += 10 * dt;
		camera.target.y += 10 * dt;
	}

	if (Application::IsKeyPressed(VK_DOWN))
	{
		camera.position.y -= 10 * dt;
		camera.target.y -= 10 * dt;
	}

	static bool bSpaceState = false;
	if (!bSpaceState && Application::IsKeyPressed(VK_SPACE))
	{
		//bSpaceState = true;
		////spawn small GO_BALL
		//GameObject *go = FetchGO();
		//go->active = true;
		//go->type = GameObject::GO_BLUE;

		//float m_worldSizeX = m_worldWidth;
		//float posX = m_worldSizeX * 0.9f;
		//float posY = 10;

		//go->pos.Set(posX, posY, 0);
		//go->vel.Set(0, 10, 0);
		//float sc = 5;
		//go->scale.Set(sc, sc, sc);
		//go->mass = 2.5f * 2.5f * 2.5f;
		//std::cout << go->pos << std::endl;
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

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = static_cast<float>(x) / w * m_worldWidth;
		float posY = (h - static_cast<float>(y)) / h * m_worldHeight;

		m_ghost->pos.Set(posX, posY, 0); //IMPT
		m_ghost->active = true;
		float sc = 2;
		m_ghost->scale.Set(sc, sc, sc);
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;

		//spawn small GO_BALL
		GameObject *go = FetchGO();
		go->active = true;
		go->type = GameObject::GO_BALL;
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = static_cast<float>(x) / w * m_worldWidth;
		float posY = (h - static_cast<float>(y)) / h * m_worldHeight;


		go->pos = m_ghost->pos;
		go->vel.Set(m_ghost->pos.x - posX, m_ghost->pos.y - posY, 0);
		m_ghost->active = false;
		float sc = 2;
		go->scale.Set(sc, sc, sc);
		go->mass = sc * sc * sc;
	}
	static bool bRButtonState = false;
	if (!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = static_cast<float>(x) / w * m_worldWidth;
		float posY = (h - static_cast<float>(y)) / h * m_worldHeight;

		m_ghost->pos.Set(posX, posY, 0); //IMPT
		m_ghost->active = true;
		float sc = 3;
		m_ghost->scale.Set(sc, sc, sc);
	}
	else if (bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;

		//spawn large GO_BALL
		GameObject *go = FetchGO();
		go->active = true;
		go->type = GameObject::GO_BALL;
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = static_cast<float>(x) / w * m_worldWidth;
		float posY = (h - static_cast<float>(y)) / h * m_worldHeight;

		go->pos = m_ghost->pos;
		go->vel.Set(m_ghost->pos.x - posX, m_ghost->pos.y - posY, 0);
		m_ghost->active = false;
		float sc = 3;
		go->scale.Set(sc, sc, sc);
		go->mass = sc * sc * sc;
	}

	//Physics Simulation Section
	dt *= m_speed;

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->active)
			continue;


		go->Update(dt, thePlayerInfo->pos, m_goList);

		if (go->type == GameObject::GO_BALL||go->type == GameObject::GO_PLAYER)
		{
			go->pos += go->vel * static_cast<float>(dt);
			go->vel += gravity * dt;
		}

		for (std::vector<GameObject *>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
		{
			GameObject *go2 = static_cast<GameObject *>(*it2);
			if (!go2->active)
				continue;

			//Exercise 1: move collision code to CheckCollision() -OK!
			GameObject *goA = go, *goB = go2;
			//Practical 4, Exercise 13: improve collision detection algorithm
			if (go->type != GameObject::GO_PLAYER && go->type != GameObject::GO_BALL)
			{
				if (go2->type != GameObject::GO_PLAYER && go2->type != GameObject::GO_BALL)
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

	//Enemy Paddle AI
	//m_enemy->pos.y = m_paddle->pos.y;

	if (m_plives <= 0)
	{
		Application::SetScene(3);
	}

	if (m_elives <= 0)
	{
		Application::SetScene(4);
	}

}


void SceneCollision::RenderGO(GameObject *go)
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
	case GameObject::GO_PLAYER:
		modelStack.PushMatrix();
		modelStack.Translate(thePlayerInfo->pos.x, thePlayerInfo->pos.y + 5, thePlayerInfo->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("player_right_" + std::to_string(playerMoveIndex)), false);
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
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("cube"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PADDLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, m_paddle->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);// normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("cube"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PILLAR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("blue"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_MELEE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("blue"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_RANGED:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("blue"), false);
		modelStack.PopMatrix();
		break;
	}
}

void SceneCollision::Render()
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

	RenderMesh(MeshBuilder::GetInstance()->GetMesh("reference"), false);

	/*modelStack.PushMatrix();
	modelStack.Translate(thePlayerInfo->position.x, thePlayerInfo->position.y, thePlayerInfo->position.z);
	modelStack.Scale(4, 4, 1);
	RenderMesh(meshList[PLAYER], false);
	modelStack.PopMatrix();
*/
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}
	/*if (m_ghost->active)
		RenderGO(m_ghost);*/

	//On screen text
	std::ostringstream ss;
	ss << "Player lives: " << m_plives;
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 12);

	ss.str(std::string());
	ss.precision(3);
	ss << "Enemy lives: " << m_elives;
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 9);

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
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 6);

	ss.str(std::string());
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 3);

	//RenderTextOnScreen(meshList[GEO_CALIBRI], "Collision", Color(0, 1, 0), 3, 0, 0);
}

void SceneCollision::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	/*if (m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}*/
}
