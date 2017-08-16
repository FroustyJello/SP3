#include "SceneMenu.h"
#include "GL\glew.h"
#include "Application.h"
#include "SceneBase.h"
#include <sstream>

SceneMenu::SceneMenu()
{
}

SceneMenu::~SceneMenu()
{
}

void SceneMenu::Init()
{
	SceneBase::Init();
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	Math::InitRNG();

	clickpos = 0;
	c_bounceTime = 0;
}

void SceneMenu::Update(double dt)
{
	SceneBase::Update(dt);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

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
	if (clickpos >= 3)
		clickpos = 3;

	//Selection
	if (Application::IsKeyPressed(VK_RETURN) && c_bounceTime >= 10)
	{
		if (clickpos == 0)		//Play
			Application::SetScene(2);
		if (clickpos == 1)
			Application::SetScene(5);
		if (clickpos == 2)
			Application::SetScene(8);
		if (clickpos == 3)
			Application::SetScene(0);
	}

	c_bounceTime++;
}

void SceneMenu::Render()
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
	//RenderMesh(meshList[UI_BACK], false);
	//modelStack.PopMatrix();

	//On screen text
	//Exercise 5a: Render m_lives, m_score

	std::ostringstream ss;
	//Title
	ss << "DODGE";
	RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(1, 1, 1), 5, 12, 38);

	ss.str("");
	ss << "PONG";
	RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(1, 1, 1), 5, 21, 33);

	//Selection
	ss.str("");
	ss << "1 Player";
	if (clickpos == 0)
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 0.6, 1), 3, 33, 24);
	else
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 24);

	ss.str("");
	ss << "2 Player";
	if (clickpos == 1)
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 0.6, 1), 3, 33, 18);
	else
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 18);

	ss.str("");
	ss << "3 MapEditor";
	if (clickpos == 2)
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 0.6, 1), 3, 33, 12);
	else
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 12);

	ss.str("");
	ss << "Exit";
	if (clickpos == 3)
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0, 0.6, 1), 3, 33, 6);
	else
		RenderTextOnScreen(meshList[GEO_CALIBRI], ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 6);
}

void SceneMenu::Exit()
{
	SceneBase::Exit();
}
