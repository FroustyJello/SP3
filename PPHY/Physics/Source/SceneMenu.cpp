#include "SceneMenu.h"
#include "GL\glew.h"
#include "Application.h"
#include "SceneBase.h"
#include <string>
#include <sstream>
#include <fstream>

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
	swit = 0;

	x1 = m_worldWidth * 0.5;
	x2 = m_worldWidth;

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
	if (clickpos >= 5)
		clickpos = 5;

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
			Application::SetScene(5);
		if (clickpos == 4)
		{
			std::ifstream file;
			file.open("save.csv");
			file >> Application::SceneID;
			file.close();
			Application::continueGame = true;
			Application::SetScene(Application::SceneID);
		}
		if (clickpos == 5)
			Application::SetScene(0);
			//Application::SetScene(0);
	}

	x1 -= dt;
	x2 -= dt;

	if (x1 < -m_worldWidth*0.5f)
	{
		x1= m_worldWidth;
	}

	if (x2 < -m_worldWidth*0.5f)
	{
		x2 = m_worldWidth;
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

	modelStack.PushMatrix();
	modelStack.Translate(x1, m_worldHeight*0.2, -3);
	modelStack.Scale(m_worldWidth, m_worldHeight, 1);
	RenderMesh(MeshBuilder::GetInstance()->GetMesh("bgd_0"), false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(x2, m_worldHeight*0.2, -3);
	modelStack.Scale(m_worldWidth, m_worldHeight, 1);
	RenderMesh(MeshBuilder::GetInstance()->GetMesh("bgd_1"), false);
	modelStack.PopMatrix();

	//On screen text
	//Exercise 5a: Render m_lives, m_score

	std::ostringstream ss;
	//Title
	ss << "Destroy";
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(1, 1, 1), 5, 12, 38);

	ss.str("");
	ss << "Da Jun";
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(1, 1, 1), 5, 21, 33);

	ss.str("");
	ss << "Castle";
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(1, 1, 1), 5, 25, 28);

	//Selection
	ss.str("");
	ss << "New Game";
	if (clickpos == 0)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 0.6, 1), 3, 33, 24);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 24);

	ss.str("");
	ss << "Level 2";
	if (clickpos == 1)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 0.6, 1), 3, 33, 21);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 21);

	ss.str("");
	ss << "MapEditor";
	if (clickpos == 2)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 0.6, 1), 3, 33, 18);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 18);

	ss.str("");
	ss << "SceneTemp";
	if (clickpos == 3)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 0.6, 1), 3, 33, 15);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 15);

	ss.str("");
	ss << "Continue";
	if (clickpos == 4)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 0.6, 1), 3, 33, 12);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 12);



	ss.str("");
	ss << "Exit";
	if (clickpos == 5)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0, 0.6, 1), 3, 33, 9);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.4, 0.4, 0.4), 3, 33, 9);
}

void SceneMenu::Exit()
{
	SceneBase::Exit();
}
