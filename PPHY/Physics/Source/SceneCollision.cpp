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
	chargeScale = 0;
	gravity.Set(0, -50.0f, 0);
	playerMoveIndex = 0;
	elapesTime = 0;
	canSave = false;
	trigger = false;
	triggered = false;
	enemycount2 = 0;
	CSV reader;

	vector<string> data;

	num_balls = 5000;
	m_lives = 5;
	m_TrailCount = 0;
	MAX_TRAIL_COUNT = 500;

	enemyCount = 0;


	//thePlayerInfo = CPlayer::GetInstance();
	thePlayerInfo->Init();
	thePlayerInfo->type = GameObject::GO_PLAYER;
	thePlayerInfo->scale.Set(4, 4, 4);
	thePlayerInfo->active = true;
	thePlayerInfo->dir.Set(1, 0, 0);


	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	m_objectCount = 0;

	m_timeEstimated1 = m_timeTaken1 = 0.f;
	bool timeStarted = false;

	if (!Application::continueGame)
		data = reader.Load(Application::levelName, data);
	else
	{
		data = reader.Load("save.csv", data);
		Application::continueGame = false;
	}

	LoadObjects(data);


	
	for (int i = 0; i < m_goList.size(); ++i)
	{
		if (m_goList[i]->type == GameObject::GO_PLAYER)
		{
			thePlayerInfo->pos = m_goList[i]->pos;
			thePlayerInfo->HP = 10;
			m_goList[i] = m_goList[m_goList.size() - 1];
			m_goList[m_goList.size() - 1] = thePlayerInfo;
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

			theEnemyInfo->SetLeftDiedIndices(14, 15);
			theEnemyInfo->SetRightDiedIndices(12, 13);

			m_goList[i] = theEnemyInfo;
			enemycount2++;
			enemyCount++;
			m_enemies.push_back(theEnemyInfo);
		}
	}



	camera.position.x = thePlayerInfo->pos.x - (m_worldWidth * 0.5f);
	camera.position.y = thePlayerInfo->pos.y - (m_worldHeight * 0.5f);

	camera.target.x = thePlayerInfo->pos.x - (m_worldWidth * 0.5f);
	camera.target.y = thePlayerInfo->pos.y - (m_worldHeight * 0.5f);


	RightScreenLimit = thePlayerInfo->pos.x + 10;
	LeftScreenLimit = thePlayerInfo->pos.x - 10;

	//m_ghost = new GameObject(GameObject::GO_BALL);


	CSoundEngine::GetInstance()->Init();
	CSoundEngine::GetInstance()->AddSound("BGM_1", "Music/THEBEST.mp3");
	CSoundEngine::GetInstance()->PlayASound("BGM_1");
}

GameObject* SceneCollision::FetchGO()
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->active)
		{
			bool found = false;

			if (go->type != GameObject::GO_ARROW && go->type != GameObject::GO_FIRE_ARROW &&go->type != GameObject::GO_ENEMY_BULLET)
				continue;
			/*for (std::vector<Enemy *>::iterator it2 = m_enemies.begin(); it2 != m_enemies.end(); ++it2)
			{
				GameObject *go2 = (Enemy *)*it2;
				if (go != go2)
					continue;
				found = true;
				break;
			}
			if (found)
				continue;*/
			go->active = true;
			++m_objectCount;
			return go;
		}
	}
	for (unsigned i = 0; i < 10; ++i)
	{
		GameObject *go = new GameObject(GameObject::GO_ARROW);
		m_goList.push_back(go);
	}
	GameObject *go = m_goList.back();
	go->active = true;
	++m_objectCount;
	return go;
}


bool SceneCollision::CheckCollision(GameObject *go1, GameObject *go2)
{

	 if ((go1->type == GameObject::GO_PLAYER 
		 || go1->type == GameObject::GO_ENEMY_MELEE
		 || go1->type == GameObject::GO_ENEMY_MELEE_2
		 || go1->type == GameObject::GO_ENEMY_RANGED
		 || go1->type == GameObject::GO_ENEMY_RANGED_2)
		 && go2->type == GameObject::GO_WALL
		 || go2->type == GameObject::GO_WALL_2
		 || go2->type == GameObject::GO_WALL_3
		 || go2->type == GameObject::GO_WALL_4)
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

	else if ((go1->type == GameObject::GO_ARROW || go1->type == GameObject::GO_FIRE_ARROW) && (go2->type == GameObject::GO_WALL 
		|| go2->type == GameObject::GO_ENEMY_MELEE
		|| go2->type == GameObject::GO_ENEMY_MELEE_2
		|| go2->type == GameObject::GO_ENEMY_RANGED
		|| go2->type == GameObject::GO_ENEMY_RANGED_2
		|| go2->type == GameObject::GO_WALL_2
		|| go2->type == GameObject::GO_WALL_3
		|| go2->type == GameObject::GO_WALL_4))
	{

		Vector3 p1 = go1->pos;
		Vector3 p2 = go2->pos;
		float r1 = go1->scale.x;
		float r2;
		if (go2->type == GameObject::GO_WALL)
			r2 = go2->scale.x - 3;
		else
			r2 = go2->scale.x + 1;

		Vector3 u1 = go1->vel;


		return (p2 - p1).LengthSquared() < (r1 + r2) * (r1 + r2) && (go2->pos - go1->pos).Dot(u1) > 0;

	}

	else if (go2->type == GameObject::GO_PLAYER && go1->type == GameObject::GO_ENEMY_BULLET)
	{
		Vector3 p1 = go1->pos;
		Vector3 p2 = go2->pos;
		float r1 = go1->scale.x;
		float r2;
			r2 = go2->scale.x + 1;

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

	 if ((go2->type == GameObject::GO_WALL
		 || go2->type == GameObject::GO_WALL_2
		 || go2->type == GameObject::GO_WALL_3
		 || go2->type == GameObject::GO_WALL_4) &&
		 (go->type == GameObject::GO_PLAYER 
		 || go->type == GameObject::GO_ENEMY_MELEE 
		 || go->type == GameObject::GO_ENEMY_MELEE_2
		 || go->type == GameObject::GO_ENEMY_RANGED
		 || go->type == GameObject::GO_ENEMY_RANGED_2))
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
		else
		{
			u = go->vel;
			N = (go2->pos - go->pos).Normalize();

			go->vel = u - 2 * u.Dot(N) * N;
		}
	}

	else if ((go2->type == GameObject::GO_WALL
		|| go2->type == GameObject::GO_WALL_2
		|| go2->type == GameObject::GO_WALL_3
		|| go2->type == GameObject::GO_WALL_4)
		&& (go->type == GameObject::GO_ARROW 
		|| go->type != GameObject::GO_FIRE_ARROW))
	{

		Vector3 u = go->vel;
		Vector3 N = (go2->pos - go->pos).Normalize();

		go->vel = (u - 2 * u.Dot(N) * N) * 0.4 + gravity;
		go->dir -= go->dir;
		go->rotation = (90, 0, 0, 1);
	}

	else if ((go2->type == GameObject::GO_ENEMY_MELEE
		|| go2->type == GameObject::GO_ENEMY_MELEE_2
		|| go2->type == GameObject::GO_ENEMY_RANGED
		|| go2->type == GameObject::GO_ENEMY_RANGED_2) && (go->type == GameObject::GO_ARROW || go->type == GameObject::GO_FIRE_ARROW))
	{
		Vector3 u = go->vel;
		Vector3 N = (go2->pos - go->pos).Normalize();

		go->active = false;
		//go2->active = false;
		go2->vel.x += go->vel.x * 0.25f;
		go2->vel.y += 10;
		go2->HP -= go->dmg;

		if (go2->HP <= 0)
		{
			enemyCount--;
		}
			
		std::cout << "ENEMY HIT" << std::endl;
	}

	else if (go2->type == GameObject::GO_PLAYER && go->type == GameObject::GO_ENEMY_BULLET)
	{
		Vector3 u = go->vel;
		Vector3 N = (go2->pos - go->pos).Normalize();

		go->active = false;
		//go2->active = false;
		go2->vel.x += go->vel.x * 0.25f;
		go2->vel.y += 10;
		go2->HP -= go->dmg;

		std::cout << "ENEMY HIT" << std::endl;
	}

}

void SceneCollision::LoadObjects(vector<string> data)
{
	string temp;
	GameObject *go;

	for (int i = 0; i < data.size(); i++)
	{
		go = FetchGO();

		
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

		if (go->type != GameObject::GO_ENEMY_MELEE && go->type != GameObject::GO_ENEMY_RANGED
			&& go->type != GameObject::GO_ENEMY_MELEE_2 && go->type != GameObject::GO_ENEMY_RANGED_2
			&& go->type != GameObject::GO_PLAYER && go->type != GameObject::GO_DOOR && go->type != GameObject::GO_DOOR2)
		{
			CCollider* Ctemp = new CCollider();
			Ctemp->SetPAABB(go->scale, go->pos);
			collisionVector.push_back(Ctemp);
			if (go->type == GameObject::GO_PLAYER)
			{
				Ctemp->SetPAABB(go->scale, go->pos);
			}
			Ctemp = nullptr;
		}
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
		/*if (go->type >= (GameObject::GAMEOBJECT_TYPE)11 && go->type <= (GameObject::GAMEOBJECT_TYPE)16)
		{
			if (go->HP <= 0)
				continue;
		}*/

		if (go->type != GameObject::GO_NONE && go->type != GameObject::GO_FIRE_ARROW && go->type != GameObject::GO_ARROW && go->HP > 0)
		{
			temp = std::to_string((int)go->type);
			file << temp << "," << go->pos.x << "," << go->pos.y << "," << go->pos.z << "," << go->scale.x << "," << go->scale.y << "," << go->dir.x << "," << go->dir.y << "," << go->HP << "," << go->dmg << std::endl;
		}
		std::cout << temp << std::endl;
	}
	file.close();
}

bool SceneCollision::CheckAABB(vector<CCollider*> vector, Vector3 MinAABB, Vector3 MaxAABB)
{
	for (int i = 0; i < vector.size(); i++)
	{
		collided = thePlayerInfo->CheckOverlap(MinAABB - Vector3(2,0,0), MaxAABB + Vector3(3,3,0), vector[i]->GetMinAABB(), vector[i]->GetMaxAABB());
		if (collided)
		{
			std::cout << i << std::endl;
			break;
		}
	}

	//collided = thePlayerInfo->CheckOverlap(MinAABB, MaxAABB, vector[19]->GetMinAABB(), vector[19]->GetMaxAABB());

	return collided;

}

void SceneCollision::Update(double dt)
{
	//thePlayerInfo = CPlayer::GetInstance();

	//std::cout << collisionVector[19]->GetMinAABB() << " " << collisionVector[20]->GetMinAABB() << std::endl;;

	SceneBase::Update(dt);
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();


	thePlayerInfo->Update(dt);

	CheckAABB(collisionVector, thePlayerInfo->GetMinAABB(), thePlayerInfo->GetMaxAABB());

	//UpdateParticles(dt);

	static bool is9pressed = false;

	if (Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (Application::IsKeyPressed('W'))
	{
	}

	static bool isFpressed = false;
	if (Application::IsKeyPressed('F') && !isFpressed && canSave)
	{
		isFpressed = true;
		SaveFile(m_goList);
	}
	else if(!Application::IsKeyPressed('F') && isFpressed)
	{
		isFpressed = false;
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
	double x, y;
	Application::GetCursorPos(&x, &y);
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	float posX = static_cast<float>(x) / w * m_worldWidth;
	float posY = (h - static_cast<float>(y)) / h * m_worldHeight;


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

	if (thePlayerInfo->HP <= 0)
	{
		Application::continueGame = true;
		Application::SetScene(3);
	}

	//Physics Simulation Section
	dt *= m_speed;

	//for (int i = 0; i < m_enemies.size(); ++i)
	//{
	//	if (m_enemies[i]->HP <= 0)
	//	{
	//		//m_enemies[i] = m_enemies[m_enemies.size() - 1];
	//		//m_enemies.pop_back();
	//		enemyCount-=1;
	//		++i;
	//	}
	//}

	for (std::vector<Enemy *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
	{
		Enemy *enemy = (Enemy *)*it;
		
		if (!enemy->active)
			continue;
		enemy->Update(dt, thePlayerInfo, m_enemies, collisionVector);
	}
	canSave = false;

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;

		if (go->pos.x > m_worldWidth + camera.position.x +15 || go->pos.x < 0 + camera.position.x - 20.f ||
			go->pos.y > m_worldHeight + camera.position.y || go->pos.y <camera.position.y)
		{
			go->active = false;

			if ((go->type == GameObject::GO_ENEMY_MELEE
				|| go->type == GameObject::GO_ENEMY_MELEE_2
				|| go->type == GameObject::GO_ENEMY_RANGED
				|| go->type == GameObject::GO_ENEMY_RANGED_2)&&go->HP>0)
			{
				if (go->pos.y < camera.position.y)
					go->HP = 0;

				if (go->HP <= 0)
					enemyCount--;
				continue;
			}

			if (go->type == GameObject::GO_PLAYER)
				Application::SetScene(3);
		}

		else if (go->type != GameObject::GO_ARROW && go->type != GameObject::GO_FIRE_ARROW && go->type != GameObject::GO_ENEMY_BULLET)
		{
			if (go->HP > 0)
				go->active = true;
		}
		if (!go->active)
			continue;

		if (go->type == GameObject::GO_DOOR)
		{
			if ((go->pos - thePlayerInfo->pos).Length() < 10)
				canSave = true;
		}

		else if (go->type == GameObject::GO_DOOR2)
		{
			if ((go->pos - thePlayerInfo->pos).Length() < 10)
			{
				if (Application::newGame)
				{
					Application::levelName = "level2.csv";
					Application::SetScene(9);
				}
				else
				{
					Application::SetScene(4);
				}
			}
		}

		if (go->type == GameObject::GO_WALL_2)
		{
			if(!triggered && trigger)
			{
				go->vel.Set(cos(Math::RandFloatMinMax(0, 360)), sin(Math::RandFloatMinMax(0, 360)));
				go->vel *= 60;
			}
			else if(triggered && trigger)
			{
				Vector3 G(0, -9.8f);
				go->vel += G * dt*10;
			}
		}
		if (!go->active)
			continue;

		for (std::vector<Enemy *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
		{
			Enemy *enemy = (Enemy *)*it;

			if (enemy->IsShooting)
			{
				GameObject* shoot = FetchGO();

				shoot->type = GameObject::GO_ENEMY_BULLET;
				shoot->pos = enemy->pos;
				shoot->pos.y += 5;
				shoot->pos.x += 5;
				if (thePlayerInfo->pos.x > enemy->pos.x)
				{
					shoot->vel = 500.f * dt;
				}
				else
				{
					shoot->vel = -500.f * dt;
				}
				shoot->dmg = 1;
				shoot->scale.Set(3, 3, 1);
				enemy->IsShooting = false;

				std::cout << shoot->dmg << std::endl;
			}
		}

		go->pos += go->vel * m_speed * dt;

		if (go->type == GameObject::GO_ENEMY_BULLET)
		{
			go->pos += go->vel * static_cast<float>(dt);
		}

		if (go->type == GameObject::GO_PLAYER)
			hpscale = (go->HP / 10 )* 40;

		if ( go->type == GameObject::GO_PLAYER
			|| go->type == GameObject::GO_ENEMY_MELEE
			|| go->type == GameObject::GO_ENEMY_MELEE_2
			|| go->type == GameObject::GO_ENEMY_RANGED
			|| go->type == GameObject::GO_ENEMY_RANGED_2)
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
		if (go->type == GameObject::GO_ENEMY_BULLET && Application::IsKeyPressed('P'))
		{
			go->active = false;
		}

		if (go->type == GameObject::GO_ARROW)
		{
			ParticleObject* ArrowParticle = new ParticleObject();
			ArrowParticle->pos = go->pos;
			ArrowParticle->scale.Set(3, 3, 0);
			ArrowParticle->type = ParticleObject_TYPE::P_ARROW_TRAIL;
		}

		for (std::vector<GameObject *>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
		{
			GameObject *go2 = static_cast<GameObject *>(*it2);
			if (!go2->active)
				continue;

			//Exercise 1: move collision code to CheckCollision() -OK!
			GameObject *goA = go, *goB = go2;
			//Practical 4, Exercise 13: improve collision detection algorithm
			if (go->type != GameObject::GO_PLAYER && go->type != GameObject::GO_ARROW && go->type != GameObject:: GO_FIRE_ARROW && go->type != GameObject::GO_ENEMY_BULLET)
			{
				if (go2->type != GameObject::GO_PLAYER && go2->type != GameObject::GO_ARROW &&  go2->type != GameObject::GO_ENEMY_MELEE && go2->type != GameObject::GO_FIRE_ARROW && go2->type != GameObject::GO_ENEMY_BULLET
					&& go2->type != GameObject::GO_ENEMY_MELEE_2
					&& go2->type != GameObject::GO_ENEMY_RANGED
					&& go2->type != GameObject::GO_ENEMY_RANGED_2)
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


	if (trigger)
	{
		triggered = true;
	}

	if (thePlayerInfo->isCharging)
	{
		chargeScale += 20 * dt;
		if (chargeScale > 18)
			chargeScale = 18;
	}
	else
	{
		chargeScale = 0;
	}

	if (enemyCount <= 0)
	{
		trigger = true;
	}

	/*if (Application::IsKeyPressed('P'))
	{
		trigger = true;
	}*/

	UpdateParticles(dt);
	if (Application::IsKeyPressed('9') && !is9pressed)
	{
		is9pressed = true;
		//m_speed = Math::Max(0.f, m_speed - 0.1f);
		Application::SetScene(1);
		/*for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
		GameObject *go = (GameObject *)*it;
		if (go->type == GameObject::GO_PLAYER)
		{
		go->HP--;
		break;
		}
		}*/
	}
	else if (!Application::IsKeyPressed('9') && is9pressed)
	{
		is9pressed = false;
	}
}

void SceneCollision::RenderGO(GameObject *go)
{
	switch (go->type)
	{
	case GameObject::GO_PLAYER:
		modelStack.PushMatrix();
		modelStack.Translate(thePlayerInfo->pos.x, thePlayerInfo->pos.y + 5, thePlayerInfo->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(thePlayer[thePlayerInfo->GetAnimationIndex()]->GetMesh(), false);
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
	case GameObject::GO_ENEMY_MELEE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y+1, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(fatEnemy[(dynamic_cast<Enemy*> (go))->GetAnimationIndex()]->GetMesh(), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_MELEE_2:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y+1, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(femaleEnemy[(dynamic_cast<Enemy*> (go))->GetAnimationIndex()]->GetMesh(), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_RANGED:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y+1, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(regularEnemy[(dynamic_cast<Enemy*> (go))->GetAnimationIndex()]->GetMesh(), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_RANGED_2:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y+1, go->pos.z);
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
	case GameObject::GO_DOOR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("Door"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);// normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("EnemyBullet"), false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_DOOR2:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("Door2"), false);
		modelStack.PopMatrix();
		break;
	}
}

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

		//RenderParticles(particle);

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

	if (Application::levelName == "level1.csv")
	{
		modelStack.PushMatrix();
		modelStack.Translate((camera.target.x) + 70, camera.target.y+50, 1);
		modelStack.Scale(m_worldWidth+5, m_worldHeight+5, 0);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("bg01"), false);
		modelStack.PopMatrix();
	}
	else if (Application::levelName == "level2.csv")
	{
		modelStack.PushMatrix();
		modelStack.Translate((camera.target.x) + 70, camera.target.y + 50, 1);
		modelStack.Scale(m_worldWidth + 5, m_worldHeight + 5, 0);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("bg02"), false);
		modelStack.PopMatrix();
	}
	else if (Application::levelName == "level3.csv")
	{
		modelStack.PushMatrix();
		modelStack.Translate((camera.target.x) + 70, camera.target.y + 50, 1);
		modelStack.Scale(m_worldWidth + 5, m_worldHeight + 5, 0);
		RenderMesh(MeshBuilder::GetInstance()->GetMesh("bg03"), false);
		modelStack.PopMatrix();
	}

	//RenderMesh(MeshBuilder::GetInstance()->GetMesh("reference"), false);
	RenderAllParticles();

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
			
		}
	}
 	for (std::vector<Enemy *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
	{
		Enemy *go = (Enemy *)*it;
		if (go->active)
		{
			//RenderGO(go);
			modelStack.PushMatrix();
			modelStack.Translate(go->pos.x, go->pos.y + 10, 1.1);
			if (go->type == GameObject::GO_ENEMY_MELEE || go->type == GameObject::GO_ENEMY_MELEE_2)
			{
				modelStack.Scale((go->HP / 10) * 15, 2, 1);
				RenderMesh(MeshBuilder::GetInstance()->GetMesh("health"), false);
			}

			else if (go->type == GameObject::GO_ENEMY_RANGED || go->type == GameObject::GO_ENEMY_RANGED_2)
			{
				modelStack.Scale((go->HP / 5) * 15, 2, 1);
				RenderMesh(MeshBuilder::GetInstance()->GetMesh("health"), false);
			}
			//modelStack.Translate(0.5, 0, 0);
			
			modelStack.PopMatrix();
		}

	}
	/*if (m_ghost->active)
		RenderGO(m_ghost);*/

	modelStack.PushMatrix();
	modelStack.Translate(camera.target.x + 19.5f, camera.target.y + 88.7, 1.1f);
	modelStack.Scale(hpscale * 1.6f, 4, 4);
	modelStack.Translate(0.5f, 0, 0);
	RenderMesh(MeshBuilder::GetInstance()->GetMesh("health"), false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.target.x + 19.5f, camera.target.y + 85, 1.1f);
	modelStack.Scale(chargeScale * 1.6f, 2, 1);
	modelStack.Translate(0.5f, 0, 0);
	RenderMesh(MeshBuilder::GetInstance()->GetMesh("charge"), false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.target.x + 45, camera.target.y + 88, 1);
	modelStack.Scale(18, 4, 4);
	RenderMesh(MeshBuilder::GetInstance()->GetMesh("player_healthbar"), false);
	modelStack.PopMatrix();

	

	////On screen te
	std::ostringstream ss;
	//ss << "Player lives: " << m_lives;
	//RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 12);

	/*ss.str(std::string());
	ss.precision(3);
	ss << "Pos: " << thePlayerInfo->pos;
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 9);

	ss.str(std::string());
	ss.precision(3);
	ss << "Num_Balls: " << num_balls;
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 6);*/

	ss.str(std::string());
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 0, 3);

	if (canSave)
	{
		ss.str(std::string());
		ss.precision(5);
		ss << "Press F to save";
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 1, 0), 3, 3, 25);
	}

}

void SceneCollision::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	/*while (m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}*/



	for (int i = m_goList.size() - 1; i >= 0; --i)
	{
		if (m_goList[i]->type != GameObject::GO_PLAYER)
		{
			delete m_goList[i];
			m_goList[i] = NULL;
			m_goList.pop_back();
		}
	}
	m_goList.clear();
	thePlayerInfo->DropInstance();

	if (thePlayerInfo->DropInstance == false)
	{
		std::cout << "failed to delete player" << std::endl;
	}

	m_enemies.clear();
	CSoundEngine::Destroy();

	/*while (m_enemies.size() > 0)
	{
		Enemy *go = m_enemies.back();
		delete go;
		m_enemies.pop_back();
	}
*/
	while (!collisionVector.empty())
	{
		delete collisionVector.back();
		collisionVector.pop_back();
	}
	collisionVector.clear();

	for (int i = particleList.size() - 1; i >= 0; --i)
	{
			delete particleList[i];
			particleList[i] = NULL;
			particleList.pop_back();
	}
	particleList.clear();
}