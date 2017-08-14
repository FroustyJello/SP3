#include "SceneHelp.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneHelp::SceneHelp()
{
}

SceneHelp::~SceneHelp()
{
}

void SceneHelp::Init()
{
	SceneBase::Init();
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
}

void SceneHelp::Update(double dt)
{
	SceneBase::Update(dt);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Selection
	if (Application::IsKeyPressed(VK_RETURN) || Application::IsKeyPressed(VK_BACK))
	{
		Application::SetScene(1);
	}
}

void SceneHelp::Render()
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

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -8);
	modelStack.Scale(110, 80, 40);
	RenderMesh(meshList[SPACE], false);
	modelStack.PopMatrix();

	//On screen text
	//Exercise 5a: Render m_lives, m_score

	std::ostringstream ss;
	//Title
	ss << "W : Speed up";
	RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 3, 3, 55);
	
	ss.str("");
	ss << "S : Slow down";
	RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 3, 3, 52);

	ss.str("");
	ss << "A : Turn Left";
	RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 3, 3, 49);

	ss.str("");
	ss << "D : Turn Right";
	RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 3, 3, 46);

	ss.str("");
	ss << "Space : Shoot";
	RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(1, 1, 1), 3, 3, 43);

	ss.str("");
	ss << "Return<<";
	RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0, 0.6, 1), 3, 55, 2);

	//Info
	ss.str("");
	ss.precision(4);
	ss << "FPS:" << fps;
	RenderTextOnScreen(meshList[GEO_BNMACHINE], ss.str(), Color(0, 1, 0), 3, 0, 0);
}

void SceneHelp::Exit()
{
	SceneBase::Exit();
}
