#include "Player.h"

//#include "MouseController.h"
//#include "KeyboardController.h"
#include "Mtx44.h"
#include "Application.h"
#include "SceneCollision.h"
#include "GameObject.h"
//#include "../Projectile/Projectile.h"
//#include "GoodiesFactory.h"
//#include "../SoundEngine.h"

// Allocating and initializing CPlayer's static data member.  
// The pointer is allocated but not the object's constructor.
CPlayer *CPlayer::s_instance = 0;

CPlayer::CPlayer(void)
	: m_dSpeed(5.0)
	, m_dAcceleration(10.0)
	, m_bJumpUpwards(false)
	, m_dJumpSpeed(10.0)
	, m_dJumpAcceleration(-10.0)
	, m_bFallDownwards(false)
	, RL(false)
	, m_dFallSpeed(0.0)
	, m_dFallAcceleration(-10.0)
	, m_dElapsedTime(0.0)
	, m_dElapsedDieTime(0.0)
	, playerMoveIndex(0)
	, isShooting(false)
	, arrowdmg (1)
	, chargearrow(false)
	, arrowSpeed(50.f)
	, isCharging(false)
{
}

CPlayer::~CPlayer(void)
{
}

// Initialise this class instance
void CPlayer::Init(void)
{
	// Set the default values
	defaultPosition.Set(0, 0, 10);
	defaultTarget.Set(0, 0, 0);
	defaultUp.Set(0, 1, 0);

	// Set the current values
	position.Set(0, 0, 10);
	target.Set(0, 0, 0);
	up.Set(0, 1, 0);

	// Set Boundary
	maxBoundary.Set(1, 1, 1);
	minBoundary.Set(-1, -1, -1);

	// Set default tile sizes
	tileSize_Width = 25;
	tileSize_Height = 25;

	this->SetPAABB(Vector3(4, 4, 4), GetPos());
	/*CSoundEngine::GetInstance()->Init();
	CSoundEngine::GetInstance()->AddSound("Jump", "Image//Mario-jump-sound.mp3");*/
}

// Set the boundary for the player info
void CPlayer::SetBoundary(Vector3 max, Vector3 min)
{
	maxBoundary = max;
	minBoundary = min;
}

// Set the tile size
void CPlayer::SetTileSize(const int x, const int y)
{
	tileSize_Width = x;
	tileSize_Height = y;
}

// Set the map pointer to this class
//void CPlayer::SetMap(CMap* m_cMap)
//{
//	theMapReference = m_cMap;
//}

// Returns true if the player is on ground
bool CPlayer::isOnGround(void)
{
	if (m_bJumpUpwards == false && m_bFallDownwards == false)
		return true;

	return false;
}

// Returns true if the player is jumping upwards
bool CPlayer::isJumpUpwards(void)
{
	if (m_bJumpUpwards == true && m_bFallDownwards == false)
		return true;

	return false;
}

// Returns true if the player is on freefall
bool CPlayer::isFreeFall(void)
{
	if (m_bJumpUpwards == false && m_bFallDownwards == true)
		return true;

	return false;
}

// Set the player's status to free fall mode
void CPlayer::SetOnFreeFall(bool isOnFreeFall)
{
	if (isOnFreeFall == true)
	{
		m_bJumpUpwards = false;
		m_bFallDownwards = true;
		m_dFallSpeed = 0.0;
	}
}

// Set the player to jumping upwards
void CPlayer::SetToJumpUpwards(bool isOnJumpUpwards)
{
	if (isOnJumpUpwards == true)
	{
		m_bJumpUpwards = true;
		m_bFallDownwards = false;
		m_dJumpSpeed = 10.0;

		//CSoundEngine::GetInstance()->PlayASound("Jump");
	}
}

// Set position
void CPlayer::SetPos(const Vector3& pos)
{
	position = pos;
}

// Set target
void CPlayer::SetTarget(const Vector3& target)
{
	this->target = target;
}

// Set position
void CPlayer::SetUp(const Vector3& up)
{
	this->up = up;
}

// Set m_dJumpAcceleration of the player
void CPlayer::SetJumpAcceleration(const double m_dJumpAcceleration)
{
	this->m_dJumpAcceleration = m_dJumpAcceleration;
}

// Set Fall Acceleration of the player
void CPlayer::SetFallAcceleration(const double m_dFallAcceleration)
{
	this->m_dFallAcceleration = m_dFallAcceleration;
}

// Stop the player's movement
void CPlayer::StopVerticalMovement(void)
{
	m_bJumpUpwards = false;
	m_bFallDownwards = false;
}

// Reset this player instance to default
void CPlayer::Reset(void)
{
	// Set the current values to default values
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;

	// Stop vertical movement too
	StopVerticalMovement();
}

// Get position x of the player
Vector3 CPlayer::GetPos(void)
{
	return pos;
}

// Get target
Vector3 CPlayer::GetTarget(void) const
{
	return target;
}
// Get Up
Vector3 CPlayer::GetUp(void) const
{
	return up;
}

// Get m_dJumpAcceleration of the player
double CPlayer::GetJumpAcceleration(void) const
{
	return m_dJumpAcceleration;
}

// Set Tile Offset for x-axis
int CPlayer::GetTileOffset_x(void) const
{
	return tileOffset_x;
}

// Get Max Boundary
Vector3 CPlayer::GetMaxBoundary() const
{
	return maxBoundary;
}
// Get Min Boundary
Vector3 CPlayer::GetMinBoundary() const
{
	return minBoundary;
}

/********************************************************************************
Update
********************************************************************************/
void CPlayer::Update(double dt)
{
	float m_speed = 1.0f;

	m_dElapsedDieTime += dt;
	m_dElapsedTime += dt;
	// Update the player position
	if (Application::IsKeyPressed('W'))
	{
		pos.y += 100 * dt * m_speed;
	}

	if (Application::IsKeyPressed('S'))
	{
		pos.y -= 20 * dt * m_speed;
	}

	if (Application::IsKeyPressed('D') && !Application::IsKeyPressed(VK_SPACE))
	{
		RL = false;
		pos.x += 25 * dt * m_speed;
		SetAnimationStatus(RL, true,false,false, dt);
		this->dir.x = 1;
	}

	if (Application::IsKeyPressed('A') && !Application::IsKeyPressed(VK_SPACE))
	{
		RL = true;
		pos.x -= 25 * dt * m_speed;
		SetAnimationStatus(RL, true,false,false, dt);
		this->dir.x = -1;
	}


	if (this->HP <= 0)
	{
		SetAnimationStatus(RL, false, false, true, dt);
		if (m_dElapsedDieTime >= 0.3f)
		{
			std::cout << "died" << std::endl;
			//this->active = false;
			m_dElapsedDieTime = 0.f;
		}
	}
	else if (!Application::IsKeyPressed('A') && !Application::IsKeyPressed('D') && !Application::IsKeyPressed(VK_SPACE))
		SetAnimationStatus(RL, false,false,false, dt);

	static bool isSpacepressed = false;
	if (Application::IsKeyPressed(VK_SPACE) && !isSpacepressed)
	{
		isSpacepressed = true;
		arrowdmg = 1;
		chargearrow = true;
		arrowSpeed = 50.f;
		SetAnimationStatus(RL, false, true,false, dt);
	}
	else if (!Application::IsKeyPressed(VK_SPACE) && isSpacepressed)
	{
		if (m_dElapsedTime > 0.7)
		{
			isShooting = true;
			m_dElapsedTime = 0;
		}
		
		chargearrow = false;
		isCharging = false;
		isSpacepressed = false;
	}

	if (chargearrow)
	{
		isCharging = true;
		arrowdmg += 1 * dt;
		arrowSpeed += 50 * dt;
		if (arrowdmg > 5)
			arrowdmg = 5;
	}

	

	this->SetPAABB(Vector3(4, 4, 4), GetPos());
	Constrain();
}
	

void CPlayer::UpdateSideMovements(void)
{
}

/********************************************************************************
Move Up Down
********************************************************************************/
void CPlayer::MoveUpDown(const bool mode, const float timeDiff)
{
	if (mode)
	{
		position.y = position.y + (int)(m_dSpeed * timeDiff);
	}
	else
	{
		position.y = position.y - (int)(m_dSpeed * timeDiff);
	}
}

/********************************************************************************
Move Left Right
********************************************************************************/
void CPlayer::MoveLeftRight(const bool mode, const float timeDiff)
{
	if (mode)
	{
		position.x = position.x - (int)(m_dSpeed * timeDiff);
		//SetAnimationStatus(true);
		//UpdateAnimationIndex();
	}
	else
	{
		position.x = position.x + (int)(m_dSpeed * timeDiff);
		//SetAnimationStatus(false);
		//UpdateAnimationIndex();
	}

	UpdateSideMovements();

}



 //Constrain the position within the borders
void CPlayer::Constrain(void)
{
	// Constrain player within the boundary
	/*if (position.x > maxBoundary.x - (tileSize_Width >> 1))
	{
		position.x = maxBoundary.x - (tileSize_Width >> 1);
		mapOffset_x = mapOffset_x + m_dSpeed;
		if (mapOffset_x > maxBoundary.x + tileSize_Width)
			mapOffset_x = maxBoundary.x + tileSize_Width;
	}
	if (position.y > maxBoundary.y - tileSize_Height)
	{
		position.y = maxBoundary.y - (tileSize_Height >> 1);
	}
	if (position.x < minBoundary.x - (tileSize_Width >> 1))
	{
		position.x = minBoundary.x - (tileSize_Width >> 1);
		mapOffset_x = mapOffset_x - m_dSpeed;
		if (mapOffset_x < minBoundary.x)
			mapOffset_x = 0;
	}
	if (position.y < minBoundary.y)
	{
		position.y = minBoundary.y + (tileSize_Height >> 1);
	}*/
}

//// Set mapFineOffset_x values
//void CPlayer::UpdateMapFineOffset(void)
//{
//	mapFineOffset_x = mapOffset_x % theMapReference->GetTileSize_Width();
//}
//
//// Get mapFineOffset_x values
//int CPlayer::GetMapFineOffset_x(void) const
//{
//	return mapFineOffset_x;
//}
//
///*
//For Parallax Scrolling
//*/
//
//// Set the rear map pointer to this class
//void CPlayer::SetRearMap(CMap* m_cRearMap)
//{
//	theRearMapReference = m_cRearMap;
//}
//
//// Update the rear map
//void CPlayer::UpdateRearMap(void)
//{
//	rearMapOffset_x = (int)(mapOffset_x / 2);
//	rearTileOffset_x = (int)(rearMapOffset_x / theRearMapReference->GetTileSize_Width());
//	if (rearTileOffset_x + theRearMapReference->GetNumOfTiles_Width() >
//		theRearMapReference->getNumOfTiles_MapWidth())
//		rearTileOffset_x = theRearMapReference->getNumOfTiles_MapWidth() -
//		theRearMapReference->GetNumOfTiles_Width();
//	rearMapFineOffset_x = rearMapOffset_x % theRearMapReference->GetTileSize_Width();
//}
//
//// Get Tile Offset for x-axis
//int CPlayer::GetRearTileOffset_x(void) const
//{
//	return rearTileOffset_x;
//}
//
//// Get the rear map fine offset X value
//int  CPlayer::GetRearMapFineOffset_x(void) const
//{
//	return rearMapFineOffset_x;
//}
//
//// Methods to tokenize a string into a specific data type variable
//Vector3 CPlayer::Token2Vector(string token)
//{
//	Vector3 tempVector(0.0f, 0.0f, 0.0f);
//
//	std::istringstream ss(token);
//	std::string aToken = "";
//	std::getline(ss, aToken, ',');
//	tempVector.x = Token2Double(aToken);
//	std::getline(ss, aToken, ',');
//	tempVector.y = Token2Double(aToken);
//	std::getline(ss, aToken, ',');
//	tempVector.z = Token2Double(aToken);
//
//	return tempVector;
//}
//double CPlayer::Token2Double(const string token)
//{
//	return atof(token.c_str());
//}
//bool CPlayer::Token2Bool(const string token)
//{
//	return token.at(0) == '1';
//}
//
//


