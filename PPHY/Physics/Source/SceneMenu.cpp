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
		{
			Application::continueGame = false;
			Application::newGame = true;
			Application::levelName = "level1.csv";
			Application::SetScene(2);
		}

		if (clickpos == 1)
		{
			Application::continueGame = false;
			Application::levelName = "level3.csv";
			Application::SetScene(2);
		}
		if (clickpos == 2)
			Application::SetScene(8);
		if (clickpos == 3)
		{
			std::ifstream file;
			file.open("save.csv");
			file >> Application::SceneID;
			file.close();
			Application::continueGame = true;
			Application::SetScene(Application::SceneID);
		}
		if (clickpos == 4)
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
	modelStack.Translate(x1, m_worldHeight*0.4, -3);
	modelStack.Scale(m_worldWidth, m_worldHeight, 1);
	RenderMesh(MeshBuilder::GetInstance()->GetMesh("bgd_0"), false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(x2+66, m_worldHeight*0.4, -3);
	modelStack.Scale(m_worldWidth, m_worldHeight, 1);
	RenderMesh(MeshBuilder::GetInstance()->GetMesh("bgd_1"), false);
	modelStack.PopMatrix();

	//On screen text
	//Exercise 5a: Render m_lives, m_score

	std::ostringstream ss;
	//Title
	ss << "Destroy";
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(1, 1, 1), 5, 12, 50);

	ss.str("");
	ss << "Ja Doom";
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(1, 1, 1), 5, 21, 45);

	ss.str("");
	ss << "Castle";
	RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(1, 1, 1), 5, 36, 40);

	//Selection
	ss.str("");
	ss << "New Game";
	if (clickpos == 0)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.5, 0.7, 0.5), 3, 33, 34);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.6, 0.6, 0.6), 3, 33, 34);

	ss.str("");
	ss << "Play your level";
	if (clickpos == 1)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.5, 0.7, 0.5), 3, 33, 31);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.6, 0.6, 0.6), 3, 33, 31);

	ss.str("");
	ss << "MapEditor";
	if (clickpos == 2)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.5, 0.7, 0.5), 3, 33, 28);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.6, 0.6, 0.6), 3, 33, 28);

	ss.str("");
	ss << "Continue";
	if (clickpos == 3)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.5, 0.7, 0.5), 3, 33, 25);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.6, 0.6, 0.6), 3, 33, 25);



	ss.str("");
	ss << "Exit";
	if (clickpos == 4)
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.5, 0.7, 0.5), 3, 33, 22);
	else
		RenderTextOnScreen(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(0.6, 0.6, 0.6), 3, 33, 22);
}

void SceneMenu::Exit()
{
	SceneBase::Exit();
}
