#include "SceneAsteroid.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

#include <iostream>
using namespace std;

SceneAsteroid::SceneAsteroid()
{
}

SceneAsteroid::~SceneAsteroid()
{
}

void SceneAsteroid::Init()
{
	SceneBase::Init();
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	//Exercise 2a: Construct 100 GameObject with type GO_ASTEROID and add into m_goList
	for (unsigned i = 0; i < 200; i++)
	{
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID_BIG));
		m_goEnemy.push_back(new GameObject(GameObject::GO_ENEMY));
	}

	//Exercise 2b: Initialize m_lives and m_score
	m_lives = 10;
	m_score = 0;
	p_lives = 10;

	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_SHIP);
	m_ship->active = true;
	m_ship->pos.Set(m_worldWidth * 0.1f, m_worldHeight * 0.5f, 1);
	m_ship->scale.Set(1.5, 1.5, 1.5);
	m_ship->momentOfInertia = 1.f;
	m_ship->angularVelocity = 0.f;
	m_ship->dir.Set(1, 0, 0);

	p_planet = new GameObject(GameObject::GO_PLANET);
	p_planet->active = true;
	p_planet->pos.Set(0, m_worldHeight * 0.5f, 0);
	p_planet->scale.Set(10, 10, 10);
	p_planet->momentOfInertia = 1.f;
	p_planet->angularVelocity = 0.f;
	p_planet->dir.Set(0, 0, 0);

	m_speed = 1.0f;
	m_currentTime = 0;
	m_bounceTime = 0;
	e_spawnTime = 1;

	//Waves
	start = false;
	s_timer = 0;

	w_wave1 = false;	//current wave
	d_wave1 = false;
	w_clear1 = false;	//wave clear
	d_clear1 = false;
	w_time1 = 120 *60;	

	w_wave2 = false;	//current wave
	d_wave2 = false;
	w_clear2 = false;	//wave clear
	d_clear2 = false;
	w_time2 = 120 *60;

	w_wave3 = false;	//current wave
	d_wave3 = false;
	w_clear3 = false;	//wave clear
	d_clear3 = false;
	w_time3 = 300 * 60;		//300seconds


	//Pause
	paused = false;
	clickpos = 0;
	c_bounceTime = 0;

	game_Victory = false;
}

void SceneAsteroid::Update(double dt)
{
	SceneBase::Update(dt);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	if (!start && !game_Victory)
	{
		if (Application::IsKeyPressed(VK_RETURN))
			start = true;
	}
	else if (game_Victory)
	{
		//choose
		if ((Application::IsKeyPressed('W') || Application::IsKeyPressed(VK_UP)) && c_bounceTime >= 10)
		{
			clickpos--;
			c_bounceTime = 0;
		}
		if ((Application::IsKeyPressed('S') || Application::IsKeyPressed(VK_DOWN)) && c_bounceTime >= 10)
		{
			clickpos++;
			c_bounceTime = 0;
		}

		//Lock
		if (clickpos <= 0)
			clickpos = 0;
		if (clickpos >= 1)
			clickpos = 1;

		//Selection
		if (Application::IsKeyPressed(VK_RETURN) && c_bounceTime >= 10)
		{
			if (clickpos == 0)	//Main Menu
				Application::SetScene(1);
			else if (clickpos == 1)	//Exit
				Application::SetScene(0);
		}

		c_bounceTime++;

	}
	else
	{
		if (paused)
		{
			//choose
			if ((Application::IsKeyPressed('W') || Application::IsKeyPressed(VK_UP)) && c_bounceTime >= 10)
			{
				clickpos--;
				c_bounceTime = 0;
			}
			if ((Application::IsKeyPressed('S') || Application::IsKeyPressed(VK_DOWN)) && c_bounceTime >= 10)
			{
				clickpos++;
				c_bounceTime = 0;
			}

			//Lock
			if (clickpos <= 0)
				clickpos = 0;
			if (clickpos >= 2)
				clickpos = 2;

			//Selection
			if (Application::IsKeyPressed(VK_RETURN) && c_bounceTime >= 10)
			{
				if (clickpos == 0)		//Continue
					paused = false;
				else if (clickpos == 1)	//Main Menu
					Application::SetScene(1);
				else if (clickpos == 2)	//Exit
					Application::SetScene(0);
			}

			c_bounceTime++;
		}
		else
		{
			m_currentTime += dt;
			m_shipAngle = (atan2(m_ship->dir.x, m_ship->dir.y) * -180 / PI);

			//if (Application::IsKeyPressed('9'))
			//{
			//	m_speed = Math::Max(0.f, m_speed - 0.1f);
			//}
			//if (Application::IsKeyPressed('0'))
			//{
			//	m_speed += 0.1f;
			//}

			m_force.SetZero();
			m_torque.SetZero();

			//Exercise 6: set m_force values based on WASD
			if (Application::IsKeyPressed('W'))
			{
				m_force += m_ship->dir * 100.f;
			}
			if (Application::IsKeyPressed('A'))
			{
				Vector3 r(1, -1, 0), F(0, 5, 0);
				m_force += m_ship->dir * 10.f;
				m_torque = r.Cross(F);
			}
			if (Application::IsKeyPressed('S'))
			{
				m_force += m_ship->dir * -10.f;
			}
			if (Application::IsKeyPressed('D'))
			{
				Vector3 r(-1, -1, 0), F(0, 5, 0);
				m_force += m_ship->dir * 10.f;
				m_torque = r.Cross(F);
			}

			//Exercise 8: use 2 keys to increase and decrease mass of ship
			if (Application::IsKeyPressed(VK_OEM_PLUS))
			{
				m_ship->mass = Math::Min(m_ship->mass + 0.1f, 50.f);
			}
			if (Application::IsKeyPressed(VK_OEM_MINUS))
			{
				m_ship->mass = Math::Max(m_ship->mass - 0.1f, 0.1f);
			}


			/************************************************************
				Spawning of game obj
			************************************************************/
			unsigned int number = 0;
			unsigned int enemy = 0;

			//Bullet 
			if (Application::IsKeyPressed(VK_SPACE) && m_currentTime > m_bounceTime)
			{
				for (unsigned i = 0; i < m_goList.size(); i++)
				{
					if (!m_goList[i]->active)
					{
						m_goList[i]->active = true;
						m_goList[i]->type = GameObject::GO_BULLET;
						m_goList[i]->scale.Set(0.5, 0.5, 0.5);
						m_goList[i]->pos.Set(m_ship->pos.x, m_ship->pos.y, m_ship->pos.z);
						if (m_ship->dir.IsZero())
						{
							m_goList[i]->dir.Set(BULLET_SPEED, 0, 0);
						}
						else
						{
							m_goList[i]->vel = m_ship->dir.Normalized() * BULLET_SPEED;
						}
						break;
					}
				}
				//Exercise 16: limit the spawn rate of bullets
				m_bounceTime = m_currentTime + 0.1;
			}

			//Wave 1
			if (!w_wave1 && s_timer <= 50 && !w_clear1)
			{
				s_timer++;
				d_wave1 = true;
			}
			else if (!w_clear1)
				w_wave1 = true;
			else if (w_clear1)
			{
				w_wave1 = false;
			}
			//Wave 2
			if (!w_wave2 && s_timer <= 50 && w_clear1 && !w_clear2)
			{
				s_timer++;
				d_wave2 = true;
			}
			else if (!w_clear2 && w_clear1)
				w_wave2 = true;
			else if (w_clear2)
			{
				w_wave2 = false;
			}
			//Wave 3
			if (!w_wave3 && s_timer <= 50 && w_clear1 && w_clear2 && !w_clear3)
			{
				s_timer++;
				d_wave3 = true;
			}
			else if (!w_clear3 && w_clear2 && w_clear1)
				w_wave3 = true;
			else if (w_clear3)
			{
				w_wave3 = false;
			}

			//Spawning Wave
			if (w_wave1)
			{
				d_wave1 = false;

				if (e_spawnTime % 100 == 0)
					SpawnMissle(enemy);

				if (e_spawnTime % 150 == 0)
					SpawnAsteroids(number);


				w_time1 -= 1;

				if (w_time1 <= 0)
				{
					w_clear1 = true;
					s_timer = 0;

					for (int i = 0; i < m_goList.size(); ++i)
					{
						if (m_goList[i]->active == true)
							m_goList[i]->active = false;
					}
					for (int i = 0; i < m_goEnemy.size(); ++i)
					{
						if (m_goEnemy[i]->active == true)
							m_goEnemy[i]->active = false;
					}
				}
			}
			else if (w_wave2)
			{
				d_wave2 = false;

				if (e_spawnTime % 200 == 0)
					SpawnMissle(enemy);

				if (e_spawnTime % 100 == 0)
					SpawnEnemies(enemy);

				if (e_spawnTime % 150 == 0)
					SpawnAsteroids(number);


				w_time2 -= 1;

				if (w_time2 <= 0)
				{
					w_clear2 = true;
					s_timer = 0;

					for (int i = 0; i < m_goList.size(); ++i)
					{
						if (m_goList[i]->active == true)
							m_goList[i]->active = false;
					}
					for (int i = 0; i < m_goEnemy.size(); ++i)
					{
						if (m_goEnemy[i]->active == true)
							m_goEnemy[i]->active = false;
					}
				}
			}
			else if (w_wave3)
			{
				d_wave3 = false;

				if (e_spawnTime % 150 == 0)
					SpawnMissle(enemy);

				if (e_spawnTime % 100 == 0)
					SpawnEnemies(enemy);

				if (e_spawnTime % 200 == 0)
					SpawnAsteroids(number);


				w_time3 -= 1;

				if (w_time3 <= 0)
				{
					w_clear3 = true;
					s_timer = 0;

					for (int i = 0; i < m_goList.size(); ++i)
					{
						if (m_goList[i]->active == true)
							m_goList[i]->active = false;
					}
					for (int i = 0; i < m_goEnemy.size(); ++i)
					{
						if (m_goEnemy[i]->active == true)
							m_goEnemy[i]->active = false;
					}
				}
			}

			if (w_clear3)
				game_Victory = true;


			/************************************************************
				Mouse Section
			************************************************************/
			static bool bLButtonState = false;
			if (!bLButtonState && Application::IsMousePressed(0))
			{
				bLButtonState = true;
				std::cout << "LBUTTON DOWN" << std::endl;
			}
			else if (bLButtonState && !Application::IsMousePressed(0))
			{
				bLButtonState = false;
				std::cout << "LBUTTON UP" << std::endl;
			}
			static bool bRButtonState = false;
			if (!bRButtonState && Application::IsMousePressed(1))
			{
				bRButtonState = true;
				std::cout << "RBUTTON DOWN" << std::endl;
			}
			else if (bRButtonState && !Application::IsMousePressed(1))
			{
				bRButtonState = false;
				std::cout << "RBUTTON UP" << std::endl;
			}

			/************************************************************
				Physics Simulation Section
			************************************************************/
			//Exercise 7: Update ship's velocity based on m_force
			m_ship->vel += (m_force * (1.f / m_ship->mass)) * dt * m_speed;
			m_ship->pos += m_ship->vel * dt * m_speed;

			m_ship->angularVelocity += m_torque.z *(1.f / m_ship->momentOfInertia) * dt * m_speed;


			float theta = m_ship->angularVelocity * dt * m_speed;
			m_ship->dir.Set(m_ship->dir.x * cos(theta) - m_ship->dir.y * sin(theta), m_ship->dir.x * sin(theta) + m_ship->dir.y * cos(theta), 0);


			//Exercise 10: cap the MAX_SPEED
			if (m_ship->vel.Length() > MAX_SPEED)
			{
				m_ship->vel.Normalize();
				m_ship->vel *= MAX_SPEED;
			}

			m_force = 0;

			if (m_ship->angularVelocity >= 5)
				m_ship->angularVelocity = 5;

			//Exercise 9: wrap ship position if it leaves screen
			if (m_ship->pos.x > m_worldWidth)
			{
				m_ship->pos.x = m_worldWidth;
			}
			else if (m_ship->pos.x < 0)
			{
				m_ship->pos.x = 0;
			}
			if (m_ship->pos.y > m_worldHeight)
			{
				m_ship->pos.y = 0;
			}
			else if (m_ship->pos.y < 0)
			{
				m_ship->pos.y = m_worldHeight;
			}

			//asteroid and others
			for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject *go = (GameObject *)*it;
				if (go->active)
				{
					//collision between GO_SHIP and GO_ASTEROID using simple distance-based check
					go->pos += go->vel * dt * m_speed;

					//Small Asteroid
					if (go->type == GameObject::GO_ASTEROID_SMALL)
					{
						if (go->active == true && m_ship->active == true)
						{
							if ((go->pos - m_ship->pos).LengthSquared() < (go->scale.x + m_ship->scale.x) * (go->scale.x * m_ship->scale.x) * 5)
							{
								m_ship->vel.Set(0, 0, 0);
								m_lives -= 1;
								RandomSpawnPowerup(go->pos);
								go->active = false;
								break;
							}
						}
						//Exercise 18: collision check between GO_BULLET other game obj
						for (std::vector<GameObject *>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
						{
							GameObject *go2 = (GameObject *)*it2;
							if (go2->type == GameObject::GO_BULLET)
							{
								if (go2->active == true)
								{
									if ((go->pos - go2->pos).LengthSquared() < (go->scale.x + go2->scale.x) * (go->scale.x * go2->scale.x) * 3)
									{
										m_score += 50;
										go->active = false;
										go2->active = false;
										RandomSpawnPowerup(go->pos);
										break;
									}
								}
							}
						}
					}

					//papa asteroid 
					if (go->type == GameObject::GO_ASTEROID_BIG)
					{
						if (go->active == true && m_ship->active == true)
						{
							if ((go->pos - m_ship->pos).LengthSquared() < (go->scale.x + m_ship->scale.x) * (go->scale.x * m_ship->scale.x) * 5)
							{
								m_ship->vel.Set(0, 0, 0);
								m_lives -= 1;
								RandomSpawnPowerup(go->pos);
								go->active = false;

								for (unsigned i = 0; i < m_goList.size(); i++)
								{
									if (number < 2)
									{
										m_goList[i]->active = true;
										m_goList[i]->type = GameObject::GO_ASTEROID_SMALL;
										m_goList[i]->scale.Set(1, 1, 1);
										m_goList[i]->pos.Set(go->pos.x, go->pos.y, 0);
										m_goList[i]->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
										break;
									}
								}
								break;
							}
						}

						//Exercise 18: collision check between GO_BULLET other game obj
						for (std::vector<GameObject *>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
						{
							GameObject *go2 = (GameObject *)*it2;
							if (go2->type == GameObject::GO_BULLET)
							{
								if (go2->active == true)
								{
									if ((go->pos - go2->pos).LengthSquared() < (go->scale.x + go2->scale.x) * (go->scale.x * go2->scale.x) * 5)
									{
										m_score += 100;
										RandomSpawnPowerup(go->pos);
										go->active = false;
										go2->active = false;

										for (unsigned i = 0; i < m_goList.size(); i++)
										{
											if (number < 2)
											{
												if (!m_goList[i]->active)
												{
													m_goList[i]->active = true;
													m_goList[i]->type = GameObject::GO_ASTEROID_SMALL;
													m_goList[i]->scale.Set(1, 1, 1);
													m_goList[i]->pos.Set(go->pos.x, go->pos.y, 0);
													m_goList[i]->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
													number++;
												}

											}
										}
										break;
									}
								}
							}
						}
					}

					//Powerup collsion
					if (go->type == GameObject::GO_POWERUP_HEALTH)
					{
						float p_speed = 20;
						if (go->pos.x >= m_worldWidth - 1 || go->pos.x <= 1)
						{
							go->vel.x *= -1;
						}
						if (go->pos.y >= m_worldHeight - 1 || go->pos.y <= 1)
						{
							go->vel.y *= -1;
						}

						if (go->active == true && m_ship->active == true)
						{
							if ((go->pos - m_ship->pos).LengthSquared() < (go->scale.x + m_ship->scale.x) * (go->scale.x * m_ship->scale.x) * 3)
							{
								m_lives += 1;
								p_lives += 1;
								m_score += 10;
								go->active = false;
								break;
							}
						}

						if((go->pos - m_ship->pos).Length() < 30)
						{
							if (go->pos.x < m_ship->pos.x)
								go->pos.x += m_speed * dt * p_speed;
							if (go->pos.y < m_ship->pos.y)
								go->pos.y += m_speed * dt * p_speed;
							if (go->pos.y > m_ship->pos.y)
								go->pos.y -= m_speed * dt * p_speed;
							if (go->pos.x > m_ship->pos.x)
								go->pos.x -= m_speed * dt * p_speed;
						}
					}

					//Exercise 17: unspawn bullets when they leave screen
					if (go->type == GameObject::GO_BULLET)
					{
						if (go->pos.x > m_worldWidth)
						{
							go->active = false;
						}
						else if (go->pos.x < 0)
						{
							go->active = false;
						}
						if (go->pos.y > m_worldHeight)
						{
							go->active = false;
						}
						else if (go->pos.y < 0)
						{
							go->active = false;
						}
					}

					if (go->type == GameObject::GO_ASTEROID_BIG || go->type == GameObject::GO_ASTEROID_SMALL)
					{
						if (go->pos.x > m_worldWidth)
						{
							go->active = false;
						}
						else if (go->pos.x < 0)
						{
							go->active = false;
						}
						if (go->pos.y > m_worldHeight)
						{
							go->pos.y = 0;
						}
						else if (go->pos.y < 0)
						{
							go->pos.y = m_worldHeight;
						}
					}
				}
			}

			//Enemy
			for (std::vector<GameObject *>::iterator it = m_goEnemy.begin(); it != m_goEnemy.end(); ++it)
			{
				GameObject *go = (GameObject *)*it;
				float e_speed = 10;
				if (go->active)
				{
					//collision between GO_SHIP and GO_ASTEROID using simple distance-based check
					go->pos += go->vel * dt * m_speed;
					if (go->type == GameObject::GO_ENEMY || go->type == GameObject::GO_MISSILE)
					{
						if (go->active == true && m_ship->active == true)
						{
							//physical collision
							if ((go->pos - m_ship->pos).LengthSquared() < (go->scale.y + m_ship->scale.y) * (go->scale.y * m_ship->scale.y) * 5)
							{
								m_ship->vel.Set(0, 0, 0);
								if (go->type == GameObject::GO_MISSILE)
									m_lives -= 3;
								else
									m_lives -= 1;
								RandomSpawnPowerup(go->pos);
								go->active = false;
								break;
							}

							//collision with bullets
							for (std::vector<GameObject *>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
							{
								GameObject *go2 = (GameObject *)*it2;
								if (go2->type == GameObject::GO_BULLET)
								{
									if (go2->active == true)
									{
										if ((go->pos - go2->pos).LengthSquared() < (go->scale.y + go2->scale.y) * (go->scale.y * go2->scale.y) * 5)
										{
											m_score += 100;
											go->active = false;
											go2->active = false;
											break;
										}
									}
								}
							}

							if (go->type == GameObject::GO_ENEMY)
							{
								//enemy chase
								if ((go->pos - m_ship->pos).LengthSquared() != 0)
								{
									float direction = (atan2(go->pos.y - m_ship->pos.y, go->pos.x - m_ship->pos.x) * 180 / PI);
									go->rotation = direction;

									if (go->pos.x < m_ship->pos.x)
										go->pos.x += m_speed * dt * e_speed;
									if (go->pos.y < m_ship->pos.y)
										go->pos.y += m_speed * dt * e_speed;
									if (go->pos.y > m_ship->pos.y)
										go->pos.y -= m_speed * dt * e_speed;
									if (go->pos.x > m_ship->pos.x)
										go->pos.x -= m_speed * dt * e_speed;
								}
								//Wrapping
								if (go->pos.x > m_worldWidth)
								{
									go->pos.x = m_worldWidth;
								}
								else if (go->pos.x < 0)
								{
									go->pos.x = 0;
								}
								if (go->pos.y > m_worldHeight)
								{
									go->pos.y = 0;
								}
								else if (go->pos.y < 0)
								{
									go->pos.y = m_worldHeight;
								}
							}

							if (go->type == GameObject::GO_MISSILE)
							{
								if ((go->pos.x - p_planet->pos.x) < 60)
								{
									float direction = (atan2(go->pos.y - p_planet->pos.y, go->pos.x - p_planet->pos.x) * 180 / PI);
									go->rotation = direction;

									if (go->pos.x < p_planet->pos.x)
										go->pos.x += m_speed * dt * e_speed;
									if (go->pos.y < p_planet->pos.y)
										go->pos.y += m_speed * dt * e_speed;
									if (go->pos.y > p_planet->pos.y)
										go->pos.y -= m_speed * dt * e_speed;
									if (go->pos.x > p_planet->pos.x)
										go->pos.x -= m_speed * dt * e_speed;
								}
								else
								{
									go->rotation = 0;
									go->pos.x -= m_speed * dt * e_speed;
								}

								//Wrapping
								if (go->pos.x > m_worldWidth)
								{
									go->pos.x = m_worldWidth;
								}
								else if (go->pos.x < 0)
								{
									go->pos.x = 0;
								}
								if (go->pos.y > m_worldHeight)
								{
									go->pos.y = 0;
								}
								else if (go->pos.y < 0)
								{
									go->pos.y = m_worldHeight;
								}
							}

						}
						if (go->active == true && p_planet->active == true && go->type == GameObject::GO_MISSILE)
						{
							if ((go->pos - p_planet->pos).LengthSquared() < (go->scale.y + p_planet->scale.y) * (go->scale.y * p_planet->scale.y) * 2)
							{
								p_lives -= 1;
								go->active = false;
								break;
							}
						}
					}
				}
			}

			//Sonic Blast
			if (Application::IsKeyPressed(VK_SHIFT)) //&& blast available
			{
				for (int i = 0; i < m_goList.size(); ++i)
				{
					if (m_goList[i]->type != GameObject::GO_POWERUP_HEALTH)
					{
						if ((m_goList[i]->pos - m_ship->pos).Length() <= 50 && m_goList[i]->active == true)
						{
							m_score += 100;
							m_goList[i]->active = false;
							RandomSpawnPowerup(m_goList[i]->pos);
						}
					}
				}
				for (int i = 0; i < m_goEnemy.size(); ++i)
				{
					if ((m_goEnemy[i]->pos - m_ship->pos).Length() <= 50 && m_goEnemy[i]->active == true)
					{
						m_score += 100;
						m_goEnemy[i]->active = false;
						RandomSpawnPowerup(m_goEnemy[i]->pos);
					}
				}
				//charge empty
			}

			if (m_lives >= 10)
				m_lives = 10;
			if (p_lives >= 10)
				p_lives = 10;

			//pause
			if (Application::IsKeyPressed('P'))
				paused = true;
			//Gameover
			if (m_lives <= 0 || p_lives <= 0)
			{
				Application::SetScene(3);
			}
		e_spawnTime++;
		}
	}

}

void SceneAsteroid::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	//RenderMesh(meshList[GEO_AXES], false);

	std::ostringstream ss;

	RenderSpace();

	if (!game_Victory)
	{
		if (start)
		{
			if (paused)
			{
				//Title
				ss << "PAUSED";
				RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 5, 25, 38);

				//Selection
				ss.str("");
				ss << "Continue";
				if (clickpos == 0)
					RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0, 0.6, 1), 3, 28, 24);
				else
					RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0.4, 0.4, 0.4), 3, 28, 24);

				ss.str("");
				ss << "Main Menu";
				if (clickpos == 1)
					RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0, 0.6, 1), 3, 26, 21);
				else
					RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0.4, 0.4, 0.4), 3, 26, 21);

				ss.str("");
				ss << "Exit";
				if (clickpos == 2)
					RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0, 0.6, 1), 3, 33, 18);
				else
					RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 18);

			}
			else
			{
				if (p_planet->active)
					RenderGO(p_planet);

				//objs
				for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
				{
					GameObject *go = (GameObject *)*it;
					if (go->active)
					{
						RenderGO(go);
					}
				}

				//Enemy
				for (std::vector<GameObject *>::iterator it = m_goEnemy.begin(); it != m_goEnemy.end(); ++it)
				{
					GameObject *go = (GameObject *)*it;
					if (go->active)
					{
						RenderGO(go);
					}
				}


				if (m_ship->active)
					RenderGO(m_ship);

			}
			//On screen text

			ss.str("");
			ss << "Hp:" << m_lives * 10;
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 0, 0), 3, 0, 9);

			ss.str("");
			ss.precision(4);
			ss << "Speed:" << sqrt(m_ship->vel.x * m_ship->vel.x + m_ship->vel.y * m_ship->vel.y);
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0, 0, 1), 3, 0, 6);

			ss.str("");
			ss << "Planet:" << p_lives * 10;
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1 - p_lives * 10 / 100, p_lives * 10 / 100, 0), 3, 0, 3);

			ss.str("");
			ss.precision(1);
			ss << "Score:" << m_score;
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0, 1, 1), 3, 0, 57);

			ss.str("");
			ss.precision(4);
			ss << "FPS:" << fps;
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0, 1, 0), 2, 0, 0);
		}
		else
		{
			ss.str("");
			ss << "Defend the planet!";
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 4, 5, 37);
			ss.str("");
			ss << "Defeat the enemy!";
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 4, 8, 31);
			//Selection
			ss.str("");
			ss << "Don't die";
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 4, 20, 24);

		}

		if (d_wave1)
		{
			ss.str("");
			ss << "Wave1";
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 6, 27, 27);
		}
		if (w_wave1)
		{
			ss.str("");
			ss << "Time:" << w_time1 / 60;
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 3, 50, 50);
		}

		if (d_wave2)
		{
			ss.str("");
			ss << "Wave2";
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 6, 27, 27);
		}
		if (w_wave2)
		{
			ss.str("");
			ss << "Time:" << w_time2 / 60;
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 3, 50, 50);
		}

		if (d_wave3)
		{
			ss.str("");
			ss << "Wave3";
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 6, 27, 27);
		}
		if (w_wave3)
		{
			ss.str("");
			ss << "Time:" << w_time3 / 60;
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 3, 50, 50);
		}
	}
	else
	{
		//Title
		ss.str("");
		ss << "VICTORY!";
		RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 5, 21, 38);

		ss.str("");
		ss << "Score:" << m_score;
		RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 3, 26, 33);

		//Selection
		ss.str("");
		ss << "Main Menu";
		if (clickpos == 0)
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0, 0.6, 1), 3, 26, 24);
		else
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0.4, 0.4, 0.4), 3, 26, 24);

		ss.str("");
		ss << "Exit";
		if (clickpos == 1)
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0, 0.6, 1), 3, 33, 21);
		else
			RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 21);
	}
}

void SceneAsteroid::RenderGO(GameObject *go)
{
	switch (go->type)
	{
	case GameObject::GO_SHIP:
	{
		//Exercise 4a: render a sphere with radius 1
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, 10);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);

		//Exercise 14b:	re-orientate the ship with velocity
		float angle = Math::RadianToDegree(atan2(m_ship->dir.y, m_ship->dir.x));

		modelStack.Rotate(angle - 90, 0, 0, 2);
		RenderMesh(meshList[PLAYER_SHIP], false);
		modelStack.PopMatrix();
	}
	break;

	case GameObject::GO_ASTEROID_BIG:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, -3);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[ASTEROID], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ASTEROID_SMALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, 0);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[ASTEROID], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PLANET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, -5);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[PLANET], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, 3.f);
		modelStack.Rotate(go->rotation + 90, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[ENEMY], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, 9.f);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[PLAYER_BULLET], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_MISSILE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, 2.f);
		modelStack.Rotate(go->rotation + 90, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[MISSILE], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_POWERUP_HEALTH:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, 1.f);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[POWERUP_HEALTH], false);
		modelStack.PopMatrix();
		break;
	}
}

void SceneAsteroid::RenderSpace()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -8);
	modelStack.Scale(110, 80, 40);
	RenderMesh(meshList[SPACE], false);
	modelStack.PopMatrix();
}

void SceneAsteroid::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if (m_ship)
	{
		delete m_ship;
		m_ship = NULL;
	}
}

void SceneAsteroid::SpawnAsteroids(unsigned number)
{	//Asteroids
	for (unsigned i = 0; i < m_goList.size(); i++)
	{
		if (number < 20)
		{
			if (!m_goList[i]->active)
			{
				m_goList[i]->active = true;
				m_goList[i]->type = GameObject::GO_ASTEROID_BIG;
				m_goList[i]->scale.Set(2, 2, 2);
				if (Math::RandIntMinMax(0, 1) == 0)
				{
					m_goList[i]->pos.Set(Math::RandFloatMinMax(m_worldWidth * 0.4, m_worldWidth), Math::RandFloatMinMax(m_worldHeight * 0.99, m_worldHeight), 0);
					m_goList[i]->vel.Set(Math::RandFloatMinMax(0, 0), Math::RandFloatMinMax(-10, -5), 0);
				}
				else
				{
					m_goList[i]->pos.Set(Math::RandFloatMinMax(m_worldWidth * 0.4, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight * 0.01), 0);
					m_goList[i]->vel.Set(Math::RandFloatMinMax(0, 0), Math::RandFloatMinMax(5, 10), 0);
				}break;
			}
			number++;
		}
	}
}

void SceneAsteroid::SpawnEnemies(unsigned enemy)
{	//Enemy
	for (unsigned i = 0; i < m_goEnemy.size(); i++)
	{
		if (enemy < 50)
		{
			if (!m_goEnemy[i]->active)
			{
				m_goEnemy[i]->active = true;
				m_goEnemy[i]->type = GameObject::GO_ENEMY;
				m_goEnemy[i]->scale.Set(1, 1, 1);
				m_goEnemy[i]->pos.Set(Math::RandFloatMinMax(m_worldWidth * 0.99f, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), 0);
				m_goEnemy[i]->vel.Set(-5, 0, 0);
				break;
			}
			enemy++;
		}
	}
}

void SceneAsteroid::SpawnMissle(unsigned enemy)
{	//Missile
	for (unsigned i = 0; i < m_goEnemy.size(); i++)
	{
		if (enemy < 50)
		{
			if (!m_goEnemy[i]->active)
			{
				m_goEnemy[i]->active = true;
				m_goEnemy[i]->type = GameObject::GO_MISSILE;
				m_goEnemy[i]->scale.Set(1, 3, 1);
				m_goEnemy[i]->pos.Set(Math::RandFloatMinMax(m_worldWidth * 0.99f, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), 0);
				m_goEnemy[i]->vel.Set(-3, 0, 0);
				break;
			}
			enemy++;
		}
	}
}

void SceneAsteroid::SpawnPowerup(Vector3 go)
{	//Powerup
	for (unsigned i = 0; i < m_goList.size(); i++)
	{
		if (!m_goList[i]->active)
		{
			m_goList[i]->active = true;
			m_goList[i]->type = GameObject::GO_POWERUP_HEALTH;
			m_goList[i]->scale.Set(1, 1, 1);
			m_goList[i]->pos.Set(go.x, go.y, 0);
			m_goList[i]->vel.Set(Math::RandFloatMinMax(-10, 10), Math::RandFloatMinMax(-10, 10), 0);
			break;
		}

	}
}

void SceneAsteroid::RandomSpawnPowerup(Vector3 go)
{	//Powerup
	if (Math::RandIntMinMax(0, 3) == 0)
		SpawnPowerup(go);
}

GameObject* SceneAsteroid::FetchGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		for (unsigned i = 0; i < 10; i++)
		{
			GameObject* go = (GameObject *)* it;
			if (!go->active)
			{
				go->active = true;
				return m_goList[m_goList.size() - 1];
			}
		}
	}
	return NULL;
}
