#include "SceneBase.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>

SceneBase::SceneBase()
{
}

SceneBase::~SceneBase()
{
}

void SceneBase::Init()
{
	thePlayerInfo = CPlayer::GetInstance();
	enemyCount = 0;
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	m_programID = LoadShaders("Shader//comg.vertexshader", "Shader//comg.fragmentshader");

	// Get a handle for our uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	//m_parameters[U_MODEL] = glGetUniformLocation(m_programID, "M");
	//m_parameters[U_VIEW] = glGetUniformLocation(m_programID, "V");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	// Use our shader
	glUseProgram(m_programID);

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 20, 0);
	lights[0].color.Set(1, 1, 1);
	lights[0].power = 1;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0].cosInner = cos(Math::DegreeToRadian(30));
	lights[0].exponent = 3.f;
	lights[0].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform1i(m_parameters[U_NUMLIGHTS], 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], lights[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], lights[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], lights[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], lights[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], lights[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], lights[0].exponent);

	camera.Init(Vector3(0, 0, 1), Vector3(0, 0, 0), Vector3(0, 1, 0));

	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
	MeshBuilder::GetInstance()->GenerateAxes("reference", 1000, 1000, 1000);
	MeshBuilder::GetInstance()->GenerateSphere("ball", Color(1, 1, 1), 10, 10, 1.f);
	MeshBuilder::GetInstance()->GenerateSphere("blue", Color(1, 0, 1), 10, 10, 1.f);
	MeshBuilder::GetInstance()->GenerateCube("cube", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//calibri.tga");
	//meshList[GEO_CALIBRI]->material.kAmbient.Set(1, 0, 0);

	MeshBuilder::GetInstance()->GenerateText("BNtext", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("BNtext")->textureID = LoadTGA("Image//BNMachine.tga");
	MeshBuilder::GetInstance()->GetMesh("BNtext")->material.kAmbient.Set(1, 0, 0);

	//Exercise 14a: render a ship texture or 3D ship model
	MeshBuilder::GetInstance()->GenerateQuad("space", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("space")->textureID = LoadTGA("Image//space.tga");

	MeshBuilder::GetInstance()->GenerateQuad("planet", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("planet")->textureID = LoadTGA("Image//planet.tga");

	MeshBuilder::GetInstance()->GenerateQuad("ship", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("ship")->textureID = LoadTGA("Image//ship.tga");

	MeshBuilder::GetInstance()->GenerateQuad("player_bullet", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_bullet")->textureID = LoadTGA("Image//player_bullet.tga");

	MeshBuilder::GetInstance()->GenerateQuad("asteroid", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("asteroid")->textureID = LoadTGA("Image//asteroid.tga");

	MeshBuilder::GetInstance()->GenerateQuad("enemy", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("enemy")->textureID = LoadTGA("Image//Enemies/Fat/fat_left_still_0.tga");

	MeshBuilder::GetInstance()->GenerateQuad("player", Color(1, 1, 1), 2.f);
	MeshBuilder::GetInstance()->GetMesh("player")->textureID = LoadTGA("Image//Player/player_right_still_0.tga");

	MeshBuilder::GetInstance()->GenerateQuad("missile", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("missile")->textureID = LoadTGA("Image//missile.tga");

	MeshBuilder::GetInstance()->GenerateQuad("powerup_health", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("powerup_health")->textureID = LoadTGA("Image//powerup_health.tga");


	MeshBuilder::GetInstance()->GenerateQuad("tile_1", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("tile_1")->textureID = LoadTGA("Image//Enviorment/tile1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("tile_2", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("tile_2")->textureID = LoadTGA("Image//Enviorment/tile2.tga");
	MeshBuilder::GetInstance()->GenerateQuad("tile_3", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("tile_3")->textureID = LoadTGA("Image//Enviorment/tile3.tga");
	MeshBuilder::GetInstance()->GenerateQuad("tile_4", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("tile_4")->textureID = LoadTGA("Image//Enviorment/tile4.tga");

	MeshBuilder::GetInstance()->GenerateQuad("player_right_face", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_right_face")->textureID = LoadTGA("Image//Player/player_right_face.tga");
	
	//Fat enemy
	MeshBuilder::GetInstance()->GenerateQuad("fat_left_still_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_left_still_0")->textureID = LoadTGA("Image//Enemies/Fat/fat_left_still_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("fat_left_still_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_left_still_1")->textureID = LoadTGA("Image//Enemies/Fat/fat_left_still_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("fat_left_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_left_0")->textureID = LoadTGA("Image//Enemies/Fat/fat_left_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("fat_left_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_left_1")->textureID = LoadTGA("Image//Enemies/Fat/fat_left_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("fat_left_att", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_left_att")->textureID = LoadTGA("Image//Enemies/Fat/fat_left_att.tga");
	MeshBuilder::GetInstance()->GenerateQuad("fat_left_died", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_left_died")->textureID = LoadTGA("Image//Enemies/Fat/fat_left_died.tga");

	MeshBuilder::GetInstance()->GenerateQuad("fat_right_still_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_right_still_0")->textureID = LoadTGA("Image//Enemies/Fat/fat_right_still_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("fat_right_still_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_right_still_1")->textureID = LoadTGA("Image//Enemies/Fat/fat_right_still_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("fat_right_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_right_0")->textureID = LoadTGA("Image//Enemies/Fat/fat_right_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("fat_right_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_right_1")->textureID = LoadTGA("Image//Enemies/Fat/fat_right_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("fat_right_att", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_right_att")->textureID = LoadTGA("Image//Enemies/Fat/fat_right_att.tga");
	MeshBuilder::GetInstance()->GenerateQuad("fat_right_died", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("fat_right_died")->textureID = LoadTGA("Image//Enemies/Fat/fat_right_died.tga");

	//Female enemy
	MeshBuilder::GetInstance()->GenerateQuad("female_left_still_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_left_still_0")->textureID = LoadTGA("Image//Enemies/Female/female_left_still_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_left_still_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_left_still_1")->textureID = LoadTGA("Image//Enemies/Female/female_left_still_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_left_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_left_0")->textureID = LoadTGA("Image//Enemies/Female/female_left_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_left_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_left_1")->textureID = LoadTGA("Image//Enemies/Female/female_left_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_left_att", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_left_att")->textureID = LoadTGA("Image//Enemies/Female/female_left_att.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_left_died", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_left_died")->textureID = LoadTGA("Image//Enemies/Female/female_left_died.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_right_still_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_right_still_0")->textureID = LoadTGA("Image//Enemies/Female/female_right_still_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_right_still_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_right_still_1")->textureID = LoadTGA("Image//Enemies/Female/female_right_still_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_right_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_right_0")->textureID = LoadTGA("Image//Enemies/Female/female_right_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_right_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_right_1")->textureID = LoadTGA("Image//Enemies/Female/female_right_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_right_att", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_right_att")->textureID = LoadTGA("Image//Enemies/Female/female_right_att.tga");
	MeshBuilder::GetInstance()->GenerateQuad("female_right_died", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("female_right_died")->textureID = LoadTGA("Image//Enemies/Female/female_right_died.tga");

	//Regular enemy
	MeshBuilder::GetInstance()->GenerateQuad("regular_left_still_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_left_still_0")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_still_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_left_still_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_left_still_1")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_still_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_left_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_left_0")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_left_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_left_1")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_left_att", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_left_att")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_att.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_left_died", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_left_died")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_died.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_right_still_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_right_still_0")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_still_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_right_still_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_right_still_1")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_still_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_right_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_right_0")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_right_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_right_1")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_right_att", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_right_att")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_att.tga");
	MeshBuilder::GetInstance()->GenerateQuad("regular_right_died", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("regular_right_died")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_died.tga");

	//Pistol enemy
	MeshBuilder::GetInstance()->GenerateQuad("pistol_left_still_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_left_still_0")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_still_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_left_still_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_left_still_1")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_still_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_left_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_left_0")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_left_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_left_1")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_left_att", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_left_att")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_att.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_left_died", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_left_died")->textureID = LoadTGA("Image//Enemies/Regular/regular_left_died.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_right_still_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_right_still_0")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_still_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_right_still_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_right_still_1")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_still_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_right_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_right_0")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_right_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_right_1")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_right_att", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_right_att")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_att.tga");
	MeshBuilder::GetInstance()->GenerateQuad("pistol_right_died", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("pistol_right_died")->textureID = LoadTGA("Image//Enemies/Regular/regular_right_died.tga");

	//Player sprite
	MeshBuilder::GetInstance()->GenerateQuad("player_right_still_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_right_still_0")->textureID = LoadTGA("Image//Player/player_right_still_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("player_right_still_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_right_still_1")->textureID = LoadTGA("Image//Player/player_right_still_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("player_right_still_2", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_right_still_2")->textureID = LoadTGA("Image//Player/player_right_still_2.tga");
	MeshBuilder::GetInstance()->GenerateQuad("player_left_still_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_left_still_0")->textureID = LoadTGA("Image//Player/player_left_still_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("player_left_still_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_left_still_1")->textureID = LoadTGA("Image//Player/player_left_still_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("player_left_still_2", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_left_still_2")->textureID = LoadTGA("Image//Player/player_left_still_2.tga");
	MeshBuilder::GetInstance()->GenerateQuad("player_right_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_right_0")->textureID = LoadTGA("Image//Player/player_right_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("player_right_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_right_1")->textureID = LoadTGA("Image//Player/player_right_1.tga");
	MeshBuilder::GetInstance()->GenerateQuad("player_left_0", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_left_0")->textureID = LoadTGA("Image//Player/player_left_0.tga");
	MeshBuilder::GetInstance()->GenerateQuad("player_left_1", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_left_1")->textureID = LoadTGA("Image//Player/player_left_1.tga");

	MeshBuilder::GetInstance()->GenerateQuad("player_healthbar", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("player_healthbar")->textureID = LoadTGA("Image//Player/player_healthBar.tga");
	MeshBuilder::GetInstance()->GenerateQuad("health", Color(1, 0, 0), 1.f);

	thePlayer = new SpriteEntity*[10];
	thePlayer[0] = Create::Sprite2DObject("player_right_0", true);
	thePlayer[1] = Create::Sprite2DObject("player_right_1", true);
	thePlayer[2] = Create::Sprite2DObject("player_left_0", true);
	thePlayer[3] = Create::Sprite2DObject("player_left_1", true);
	thePlayer[4] = Create::Sprite2DObject("player_right_still_0", true);
	thePlayer[5] = Create::Sprite2DObject("player_right_still_1", true);
	thePlayer[6] = Create::Sprite2DObject("player_right_still_2", true);
	thePlayer[7] = Create::Sprite2DObject("player_left_still_0", true);
	thePlayer[8] = Create::Sprite2DObject("player_left_still_1", true);
	thePlayer[9] = Create::Sprite2DObject("player_left_still_2", true);

	thePlayerInfo->SetRightIdleIndices(4, 6);
	thePlayerInfo->SetLeftIdleIndices(7, 9);

	thePlayerInfo->SetRightIndices(0, 1);
	thePlayerInfo->SetLeftIndices(2, 3);

	fatEnemy = new SpriteEntity*[8];
	fatEnemy[0] = Create::Sprite2DObject("fat_right_0", true);
	fatEnemy[1] = Create::Sprite2DObject("fat_right_1", true);
	fatEnemy[2] = Create::Sprite2DObject("fat_left_0", true);
	fatEnemy[3] = Create::Sprite2DObject("fat_left_1", true);
	fatEnemy[4] = Create::Sprite2DObject("fat_left_still_0", true);
	fatEnemy[5] = Create::Sprite2DObject("fat_left_still_1", true);
	fatEnemy[6] = Create::Sprite2DObject("fat_right_still_0", true);
	fatEnemy[7] = Create::Sprite2DObject("fat_right_still_1", true);
	
	femaleEnemy = new SpriteEntity*[8];
	femaleEnemy[0] = Create::Sprite2DObject("female_right_0", true);
	femaleEnemy[1] = Create::Sprite2DObject("female_right_1", true);
	femaleEnemy[2] = Create::Sprite2DObject("female_left_0", true);
	femaleEnemy[3] = Create::Sprite2DObject("female_left_1", true);
	femaleEnemy[4] = Create::Sprite2DObject("female_left_still_0", true);
	femaleEnemy[5] = Create::Sprite2DObject("female_left_still_1", true);
	femaleEnemy[6] = Create::Sprite2DObject("female_right_still_0", true);
	femaleEnemy[7] = Create::Sprite2DObject("female_right_still_1", true);

	regularEnemy = new SpriteEntity*[8];
	regularEnemy[0] = Create::Sprite2DObject("regular_right_0", true);
	regularEnemy[1] = Create::Sprite2DObject("regular_right_1", true);
	regularEnemy[2] = Create::Sprite2DObject("regular_left_0", true);
	regularEnemy[3] = Create::Sprite2DObject("regular_left_1", true);
	regularEnemy[4] = Create::Sprite2DObject("regular_left_still_0", true);
	regularEnemy[5] = Create::Sprite2DObject("regular_left_still_1", true);
	regularEnemy[6] = Create::Sprite2DObject("regular_right_still_0", true);
	regularEnemy[7] = Create::Sprite2DObject("regular_right_still_1", true);

	pistolEnemy = new SpriteEntity*[8];
	pistolEnemy[0] = Create::Sprite2DObject("pistol_right_0", true);
	pistolEnemy[1] = Create::Sprite2DObject("pistol_right_1", true);
	pistolEnemy[2] = Create::Sprite2DObject("pistol_left_0", true);
	pistolEnemy[3] = Create::Sprite2DObject("pistol_left_1", true);
	pistolEnemy[4] = Create::Sprite2DObject("pistol_left_still_0", true);
	pistolEnemy[5] = Create::Sprite2DObject("pistol_left_still_1", true);
	pistolEnemy[6] = Create::Sprite2DObject("pistol_right_still_0", true);
	pistolEnemy[7] = Create::Sprite2DObject("pistol_right_still_1", true);


	bLightEnabled = false;

}

void SceneBase::Update(double dt)
{
	//Keyboard Section
	/*if(Application::IsKeyPressed('1'))
		glEnable(GL_CULL_FACE);
	if(Application::IsKeyPressed('2'))
		glDisable(GL_CULL_FACE);
	if(Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/

	fps = (float)(1.f / dt);
}

void SceneBase::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	if (enableLight && bLightEnabled)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView = viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
}



void SceneBase::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneBase::Exit()
{
	// Cleanup VBO
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
}
