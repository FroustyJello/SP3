#include "SceneLose.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneLose::SceneLose()
{
}

SceneLose::~SceneLose()
{
}

void SceneLose::Init()
{
	SceneBase::Init();
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	Math::InitRNG();

	clickpos = 0;
	c_bounceTime = 0;
}

void SceneLose::Update(double dt)
{
	SceneBase::Update(dt);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

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
		if (clickpos == 0)		//Play
			Application::SetScene(2);
		if (clickpos == 1)
			Application::SetScene(0);
	}

	c_bounceTime++;
}

void SceneLose::Render()
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

	//modelStack.PushMatrix();
	//modelStack.Translate(m_worldWidth * 0.5, m_worldHeight * 0.5, -3);
	//modelStack.Scale(120, 80, 40);
	//RenderMesh(meshList[UI_MENU], false);
	//modelStack.PopMatrix();

	//On screen text
	//Exercise 5a: Render m_lives, m_score

	std::ostringstream ss;
	//Title
	ss << "YOU ARE";
	RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(1, 1, 1), 5, 12, 38);

	ss.str("");
	ss << "LOSE";
	RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(1, 1, 1), 5, 21, 33);

	//Selection
	ss.str("");
	ss << "Retry";
	if (clickpos == 0)
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 0.6, 1), 3, 33, 24);
	else
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 24);

	ss.str("");
	ss << "Exit";
	if (clickpos == 1)
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 0.6, 1), 3, 33, 18);
	else
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 18);
}

void SceneLose::Exit()
{
	SceneBase::Exit();
}
