#include "SceneCollision.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>
#include "SoundEngine.h"
#define integer int


SceneCollision::SceneCollision()
{
}

SceneCollision::~SceneCollision()
{
}

void SceneCollision::Init()
{
	SceneBase::Init();
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	bounce = 0;
	hpscale = 40;
	chargeScale = 20;
	gravity.Set(0, -50.0f, 0);
	playerMoveIndex = 0;
	elapesTime = 0;
	CSV reader;

	vector<string> data;

	num_balls = 5000;
	m_lives = 5;
	m_TrailCount = 0;
	MAX_TRAIL_COUNT = 500;
	
	thePlayerInfo = CPlayer::GetInstance();
	thePlayerInfo->Init();
	thePlayerInfo->type = GameObject::GO_PLAYER;
	thePlayerInfo->scale.Set(4, 4, 4);
	thePlayerInfo->active = true;
	thePlayerInfo->dir.Set(1, 0, 0);

	/*theFatEnemyInfo->type = GameObject::GO_ENEMY_MELEE;
	theFatEnemyInfo->active = true;*/


	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	m_objectCount = 0;

	m_timeEstimated1 = m_timeTaken1 = 0.f;
	bool timeStarted = false;

	if(!Application::continueGame)
	data = reader.Load("level1.csv", data);
	else
		data = reader.Load("save.csv", data);
	
	tileAABB = new CCollider();

	LoadObjects(data);

	for (int i = 0; i < m_goList.size(); ++i)
	{
		if (m_goList[i]->type == GameObject::GO_PLAYER)
		{
			thePlayerInfo->pos = m_goList[i]->pos;
			thePlayerInfo->HP = 10;
			m_goList[i] = thePlayerInfo;
			break;
		}
	}

	for (int i = 0; i < m_goList.size(); ++i)
	{
		if (m_goList[i]->type >= GameObject::GO_ENEMY_MELEE && m_goList[i]->type <= GameObject::GO_BOSS_2)
		{
			theEnemyInfo = new Enemy();
			theEnemyInfo->type = m_goList[i]->type;
			theEnemyInfo->pos = m_goList[i]->pos;
			theEnemyInfo->scale = m_goList[i]->scale;
			theEnemyInfo->HP = m_goList[i]->HP;
			theEnemyInfo->dmg = m_goList[i]->dmg;
			theEnemyInfo->active = true;

			theEnemyInfo->SetRightIndices(0, 1);
			theEnemyInfo->SetLeftIndices(2, 3);

			theEnemyInfo->SetLeftIdleIndices(4, 5);
			theEnemyInfo->SetRightIdleIndices(6, 7);

			theEnemyInfo->SetLeftAttIndices(10, 11);
			theEnemyInfo->SetRightAttIndices(8, 9);
		
			m_goList[i] = theEnemyInfo;
			enemyCount++;
			m_enemies.push_back(theEnemyInfo);
		}
	}


	RightScreenLimit = thePlayerInfo->pos.x + 10;
	LeftScreenLimit = thePlayerInfo->pos.x - 10;

	m_ghost = new GameObject(GameObject::GO_BALL);


	CSoundEngine::GetInstance()->Init();
	CSoundEngine::GetInstance()->AddSound("BGM_1", "Music/RakeHorn.mp3");
	CSoundEngine::GetInstance()->PlayASound("BGM_1");
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

//Enemy* SceneCollision::FetchEnemy()
//{
//	for (std::vector<Enemy *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
//	{
//		Enemy *enemy = (Enemy *)*it;
//		if (!enemy->active)
//		{
//			enemy->active = true;
//			++m_objectCount;
//			return enemy;
//		}
//	}
//	for (unsigned i = 0; i < 10; ++i)
//	{
//		Enemy *enemy = new Enemy(Enemy::GameObject::GO_ENEMY_MELEE);
//		m_enemies.push_back(enemy);
//	}
//	Enemy *enemy = m_enemies.back();
//	enemy->active = true;
//	++m_objectCount;
//	return enemy;
//}

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
		//relativePos += N;
		//if (relativePos.Dot(N) > 0)
		//	N = -N;

		//New box code
		Vector3 detect(Math::Clamp((b1 - w0).x, 0.f, h / 2), Math::Clamp((b1 - w0).y, 0.f, l / 2), 0);
		detect += w0;

		//Vector3 right = relativePos.Cross(Vector3(0, 0, 1));



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
		
		Vector3 distance =  go->pos - go2->pos;
		distance.x = Math::Clamp(distance.x, -go2->scale.x / 2, go2->scale.x / 2);
		distance.y = Math::Clamp(distance.y, -go2->scale.y / 2, go2->scale.y / 2);

		Vector3 right = N.Cross(Vector3(0, 0, 1));

		if (abs(distance.x) > abs(distance.y))
		{
			if (distance.Dot(right) > 0)
			{
				std::cout << "hello" << std::endl;
				N = N.Cross(Vector3(0, 0, 1));
				N = -N;
			}

			if (distance.Dot(right) < 0)
			{
				std::cout << "asd" << std::endl;
				N = N.Cross(Vector3(0, 0, 1));
			}
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

		CCollider* Ctemp = new CCollider();	
		for (int k = 0; k < 10; k++)
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
			case 8:
				go->HP = stof(temp);
				break;
			case 9:
				go->dmg = stof(temp);
				break;
			}

			if (go->dir != Vector3{ 0,0,0 })
			{
				go->dir.Normalize();
			}
			data[i].erase(0, comma + 1);
		}
		Ctemp->SetPAABB(go->scale, go->pos);
		collisionVector.push_back(Ctemp);
		Ctemp = nullptr;
	}
}

void SceneCollision::SaveFile(vector<GameObject*> List)
{
	std::ofstream file;
	string temp;
	file.open("save.csv");

	if (file.fail())
		std::cout << "File failed to open" << std::endl;

	file << Application::SceneID << std::endl;
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		temp = "";
		if (go->active && go->type != GameObject::GO_NONE)
		{
			temp = std::to_string((int)go->type);
			file << temp << "," << go->pos.x << "," << go->pos.y << "," << go->pos.z << "," << go->scale.x << "," << go->scale.y << "," << go->dir.x << "," << go->dir.y << std::endl;
		}
		std::cout << temp << std::endl;
	}
	file.close();
}

bool SceneCollision::CheckAABB(vector<CCollider*> vector, Vector3 MinAABB, Vector3 MaxAABB)
{
	for (int i = 0; i < vector.size(); i++)
	{
		collided = thePlayerInfo->CheckOverlap(MinAABB, MaxAABB, vector[i]->GetMinAABB(), vector[i]->GetMaxAABB());
		if (collided)
			break;
	}

//	collided = thePlayerInfo->CheckOverlap(MinAABB, MaxAABB, vector[19]->GetMinAABB(), vector[19]->GetMaxAABB());

	return collided;

}

void SceneCollision::Update(double dt)
{
	thePlayerInfo = CPlayer::GetInstance();

	//std::cout << collisionVector[19]->GetMinAABB() << " " << collisionVector[20]->GetMinAABB() << std::endl;;

	SceneBase::Update(dt);
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	thePlayerInfo->Update(dt);

	//std::cout << collided << std::endl;
	CheckAABB(collisionVector, thePlayerInfo->GetMinAABB(), thePlayerInfo->GetMaxAABB());


	thePlayerInfo->Update(dt);
	

	//UpdateParticles(dt);

	static bool is9pressed = false;
	if (Application::IsKeyPressed('9') && !is9pressed)
	{
		is9pressed = true;
		//m_speed = Math::Max(0.f, m_speed - 0.1f);

		for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject *go = (GameObject *)*it;
			if (go->type == GameObject::GO_PLAYER)
			{
				go->HP--;
				break;
			}
		}
	}

	else if (!Application::IsKeyPressed('9') && is9pressed)
	{
		is9pressed = false;
	}
	if (Application::IsKeyPressed('0'))
	{
		SaveFile(m_goList);
	}

	if (Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (Application::IsKeyPressed('W'))
	{
	}

	if (Application::IsKeyPressed('S'))
	{
	}

	if (Application::IsKeyPressed('D'))
	{
		//Application::GetWindowWidth() * 0.75f
		if (thePlayerInfo->pos.x > RightScreenLimit)
		{
			camera.position.x += 25 * dt * m_speed;
			camera.target.x += 25 * dt * m_speed;
			RightScreenLimit += 25 * dt * m_speed;
			LeftScreenLimit += 25 * dt * m_speed;
		}

		if (collided)
		{
			thePlayerInfo->pos.x -= 50 * dt * m_speed;
		}
	}

	if (Application::IsKeyPressed('A'))
	{
		if (thePlayerInfo->pos.x < LeftScreenLimit)
		{
			camera.position.x -= 25 * dt * m_speed;
			camera.target.x -= 25 * dt * m_speed;
			LeftScreenLimit -= 25 * dt * m_speed;
			RightScreenLimit -= 25 * dt * m_speed;
		}

		else if (collided)
		{
			thePlayerInfo->pos.x += 50 * dt * m_speed;
		}
	}

	else
	{
		thePlayerInfo->pos.x = thePlayerInfo->pos.x;
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
		camera.position.x += 100 * dt;
		camera.target.x += 100 * dt;
	}

	if (Application::IsKeyPressed(VK_LEFT))
	{
		camera.position.x -= 100 * dt;
		camera.target.x -= 100 * dt;
	}

	if (Application::IsKeyPressed(VK_UP))
	{
		camera.position.y += 100 * dt;
		camera.target.y += 100 * dt;
	}

	if (Application::IsKeyPressed(VK_DOWN))
	{
		camera.position.y -= 100 * dt;
		camera.target.y -= 100 * dt;
	}

	//static bool bSpaceState = false;
	//if (!bSpaceState && Application::IsKeyPressed(VK_SPACE))
	//{
	//	//bSpaceState = true;
	//	////spawn small GO_BALL
	//	//GameObject *go = FetchGO();
	//	//go->active = true;
	//	//go->type = GameObject::GO_BLUE;

	//	//float m_worldSizeX = m_worldWidth;
	//	//float posX = m_worldSizeX * 0.9f;
	//	//float posY = 10;

	//	//go->pos.Set(posX, posY, 0);
	//	//go->vel.Set(0, 10, 0);
	//	//float sc = 5;
	//	//go->scale.Set(sc, sc, sc);
	//	//go->mass = 2.5f * 2.5f * 2.5f;
	//	//std::cout << go->pos << std::endl;
	//}
	//else if (bSpaceState && !Application::IsKeyPressed(VK_SPACE))
	//{
	//	bSpaceState = false;
	//}

	//	double x, y;
	//	Application::GetCursorPos(&x, &y);
	//	int w = Application::GetWindowWidth();
	//	int h = Application::GetWindowHeight();
	//	float posX = static_cast<float>(x) / w * m_worldWidth;
	//	float posY = (h - static_cast<float>(y)) / h * m_worldHeight;

	////Mouse Section
	//static bool bLButtonState = false;
	//if (!bLButtonState && Application::IsMousePressed(0))
	//{
	//	bLButtonState = true;
	//	std::cout << "LBUTTON DOWN" << std::endl;


	//	m_ghost->active = true;
	//	m_ghost->pos.Set(posX, posY, 0); //IMPT
	//	float sc = 2;
	//	m_ghost->scale.Set(sc, sc, sc);
	//}
	//else if (bLButtonState && !Application::IsMousePressed(0))
	//{
	//	bLButtonState = false;
	//	std::cout << "LBUTTON UP" << std::endl;

	//	//spawn small GO_BALL
	//	GameObject *go = FetchGO();
	//	go->active = true;
	//	go->type = GameObject::GO_BALL;

	//	go->pos = m_ghost->pos;
	//	go->vel.Set(m_ghost->pos.x - posX, m_ghost->pos.y - posY, 0);
	//	m_ghost->active = false;
	//	float sc = 2;
	//	go->scale.Set(sc, sc, sc);
	//	go->mass = sc * sc * sc;
	//}
	//static bool bRButtonState = false;
	//if (!bRButtonState && Application::IsMousePressed(1))
	//{
	//	bRButtonState = true;
	//	std::cout << "RBUTTON DOWN" << std::endl;

	//	double x, y;
	//	Application::GetCursorPos(&x, &y);
	//	int w = Application::GetWindowWidth();
	//	int h = Application::GetWindowHeight();
	//	float posX = static_cast<float>(x) / w * m_worldWidth;
	//	float posY = (h - static_cast<float>(y)) / h * m_worldHeight;

	//	m_ghost->pos.Set(posX, posY, 0); //IMPT
	//	m_ghost->active = true;
	//	float sc = 3;
	//	m_ghost->scale.Set(sc, sc, sc);
	//}
	//else if (bRButtonState && !Application::IsMousePressed(1))
	//{
	//	bRButtonState = false;
	//	std::cout << "RBUTTON UP" << std::endl;

	//	//spawn large GO_BALL
	//	GameObject *go = FetchGO();
	//	go->active = true;
	//	go->type = GameObject::GO_BALL;
	//	double x, y;
	//	Application::GetCursorPos(&x, &y);
	//	int w = Application::GetWindowWidth();
	//	int h = Application::GetWindowHeight();
	//	float posX = static_cast<float>(x) / w * m_worldWidth;
	//	float posY = (h - static_cast<float>(y)) / h * m_worldHeight;

	//	go->pos = m_ghost->pos;
	//	go->vel.Set(m_ghost->pos.x - posX, m_ghost->pos.y - posY, 0);
	//	m_ghost->active = false;
	//	float sc = 3;
	//	go->scale.Set(sc, sc, sc);
	//	go->mass = sc * sc * sc;
	//}

	if (thePlayerInfo->isShooting)
	{
		GameObject* shoot = FetchGO();
		if (thePlayerInfo->arrowdmg < 3)
			shoot->type = GameObject::GO_ARROW;	
		else
			shoot->type = GameObject::GO_FIRE_ARROW;

		shoot->pos = thePlayerInfo->pos;
		shoot->pos.y += 9.3;
		shoot->pos.x += 5;
		shoot->dir = thePlayerInfo->dir;
		shoot->dmg = thePlayerInfo->arrowdmg;
		shoot->vel = thePlayerInfo->dir * thePlayerInfo->arrowSpeed;
		shoot->scale.Set(3, 3, 1);
		thePlayerInfo->isShooting = false;
	}

	//Physics Simulation Section
	dt *= m_speed;

	for (std::vector<Enemy *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
	{
		Enemy *enemy = (Enemy *)*it;
		if (!enemy->active)
			continue;
		enemy->Update(dt, thePlayerInfo, m_enemies, collisionVector);
	}

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		go->pos += go->vel * m_speed * dt;

		if (go->pos.x > m_worldWidth + camera.position.x + 3|| go->pos.x < 0 + camera.position.x - 3 ||
			go->pos.y > m_worldHeight + camera.position.y || go->pos.y < 0 + camera.position.y)
		{
			go->active = false;
		}
		else
			go->active = true;


		if (go->type == GameObject::GO_PLAYER)
			hpscale = (go->HP / 10 )* 40;
		

		if (!go->active)
			continue;

		if (go->type == GameObject::GO_BALL||go->type == GameObject::GO_PLAYER)
		{
			go->pos += go->vel * static_cast<float>(dt);
			go->vel += gravity * dt;
			if (go->vel.Length() > 40)
			{
				go->vel.Normalize();
				go->vel *= 40;
			}
		}

		//Debug code for clearing balls
		if (go->type == GameObject::GO_BALL && Application::IsKeyPressed('P'))
		{
			go->active = false;
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

	if (m_lives <= 0)
	{
		Application::SetScene(3);
	}

	UpdateParticles(dt);
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
		RenderMesh(thePlayer[thePlayerInfo->GetAnimationIndex()]->GetMesh(), false);
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
		RenderMesh(fatEnemy[(dynamic_cast<Enemy*> (go))->GetAnimationIndex()]->GetMesh(), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_MELEE_2:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(femaleEnemy[(dynamic_cast<Enemy*> (go))->GetAnimationIndex()]->GetMesh(), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_RANGED:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(regularEnemy[(dynamic_cast<Enemy*> (go))->GetAnimationIndex()]->GetMesh(), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_RANGED_2:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(pistolEnemy[(dynamic_cast<Enemy*> (go))->GetAnimationIndex()]->GetMesh(), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BOSS_1:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("blue"), false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_ARROW:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);// normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("arrow"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_FIRE_ARROW:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);// normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("player_fire_arrow"), false);
		modelStack.PopMatrix();
		break;
	}
}

//void SceneCollision::RenderEnemy(Enemy *enemy)
//{
//	switch (enemy->type)
//	{
//	case GameObject::GO_ENEMY_MELEE:
//		modelStack.PushMatrix();
//		modelStack.Translate(enemy->pos.x, enemy->pos.y, enemy->pos.z);
//		modelStack.Scale(enemy->scale.x, enemy->scale.y, enemy->scale.z);
//		RenderMesh(MeshBuilder::GetInstance()->GetMesh("blue"), false);
//		modelStack.PopMatrix();
//		break;
//	case GameObject::GO_ENEMY_RANGED:
//		modelStack.PushMatrix();
//		modelStack.Translate(enemy->pos.x, enemy->pos.y, enemy->pos.z);
//		modelStack.Scale(enemy->scale.x, enemy->scale.y, enemy->scale.z);
//		RenderMesh(MeshBuilder::GetInstance()->GetMesh("blue"), false);
//		modelStack.PopMatrix();
//		break;
//	case GameObject::GO_BOSS_1:
//		modelStack.PushMatrix();
//		modelStack.Translate(enemy->pos.x, enemy->pos.y, enemy->pos.z);
//		modelStack.Scale(enemy->scale.x, enemy->scale.y, enemy->scale.z);
//		RenderMesh(MeshBuilder::GetInstance()->GetMesh("blue"), false);
//		modelStack.PopMatrix();
//		break;
//	}
//}

ParticleObject* SceneCollision::GetParticle(void)
{
	std::vector<ParticleObject*>::iterator it, end;
	end = particleList.end();

	for (it = particleList.begin(); it != end; ++it)
	{
		ParticleObject* particle = (ParticleObject*)* it;
		if (!particle->isActive)
		{
			particle->isActive = true;
			return particle;
		}
	}

	for (unsigned i = 0; i < 10; ++i)
	{
		ParticleObject* particle = new ParticleObject(ParticleObject_TYPE::P_ARROW_TRAIL);
		particleList.push_back(particle);
	}
	ParticleObject* particle = particleList.back();
	particle->isActive = true;
	return particle;
}

void SceneCollision::UpdateParticles(double dt)
{
	if (m_TrailCount < MAX_TRAIL_COUNT)
	{
		m_TrailCount++;
		ParticleObject* particle = GetParticle();
		particle->type = ParticleObject_TYPE::P_ARROW_TRAIL;
		particle->scale.Set(1.f, 1.f, 1.f);
		particle->vel.Set(0.0f, 0.0f, 0.0f);
		particle->pos.Set(Math::RandFloatMinMax(0, 100), 0, 0);
	}

	std::vector<ParticleObject*>::iterator it, end;
	end = particleList.end();

	for (it = particleList.begin(); it != end; ++it)
	{
		ParticleObject* particle = (ParticleObject*)*it;
		if (!particle->isActive)
			continue;

		if (particle->type == ParticleObject_TYPE::P_ARROW_TRAIL)
		{
			particle->vel -= gravity * (float)dt * 0.5f;
			particle->pos += particle->vel * (float)dt * 10.f;
			particle->rotation += particle->rotationSpeed *(float)dt;

			if (particle->pos.y > 20.f)
			{
				particle->isActive = false;
				m_TrailCount--;
			}
		}
	}
}

void SceneCollision::RenderParticles(ParticleObject* particle)
{
	switch (particle->type)
	{
	case ParticleObject_TYPE::P_ARROW_TRAIL:
		modelStack.PushMatrix();
		modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
		modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("missile"), false);
		modelStack.PopMatrix();
		break;
	}
}

void SceneCollision::RenderAllParticles()
{
	std::vector<ParticleObject*>::iterator it, end;
	end = particleList.end();

	for (it = particleList.begin(); it != end; ++it)
	{
		ParticleObject* particle = (ParticleObject*)*it;
		if (!particle->isActive)
			continue;

		RenderParticles(particle);

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
	RenderAllParticles();

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}

	modelStack.PushMatrix();
	modelStack.Translate(camera.target.x + 15, camera.target.y + 88.7, 1.1);
	modelStack.Scale(hpscale, 4, 4);
	modelStack.Translate(0.5, 0, 0);
	RenderMesh(MeshBuilder::GetInstance()->GetMesh("health"), false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.target.x + 30, camera.target.y + 88, 1);
	modelStack.Scale(12, 4, 4);
	RenderMesh(MeshBuilder::GetInstance()->GetMesh("player_healthbar"), false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.target.x + 15, camera.target.y + 83, 1.1);
	modelStack.Scale(chargeScale, 4, 4);
	modelStack.Translate(0.5, 0, 0);
	RenderMesh(MeshBuilder::GetInstance()->GetMesh("health"), false);
	modelStack.PopMatrix();

	/*for (std::vector<Enemy *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
	{
		Enemy *go = (Enemy *)*it;
		if (go->active)
		{
			RenderGO(go);
		}

	}*/
	/*if (m_ghost->active)
		RenderGO(m_ghost);*/
	


	//On screen text
	std::ostringstream ss;
	ss << "Player lives: " << m_lives;
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 12);

	ss.str(std::string());
	ss.precision(3);
	ss << "Pos: " << thePlayerInfo->pos;
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 9);

	ss.str(std::string());
	ss.precision(3);
	ss << "Num_Balls: " << num_balls;
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 6);

	ss.str(std::string());
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 3);
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