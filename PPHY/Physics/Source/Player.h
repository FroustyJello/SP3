#pragma once

#include "Mtx44.h"
#include "GameObject.h"
#include "Animation.h"
#include <string>
#include <iostream>
#include "SingletonTemplate.h"
#include "Collider.h"

using std::string;


class CPlayer:public GameObject, public CAnimation, public CCollider
{
protected:
	static CPlayer *s_instance;
	CPlayer(void);

public:
	static CPlayer *GetInstance()
	{
		if (!s_instance)
			s_instance = new CPlayer;
		return s_instance;
	}
	static bool DropInstance()
	{
		if (s_instance)
		{
			delete s_instance;
			s_instance = NULL;
			return true;
		}
		return false;
	}
	~CPlayer(void);

	// Initialise this class instance
	void Init(void);
	// Set the boundary for the player info
	void SetBoundary(Vector3 max, Vector3 min);
	// Set the tile size
	void SetTileSize(const int x, const int y);
	// Set the map pointer to this class
	//void SetMap(CMap* m_cMap);

	// Returns true if the player is on ground
	bool isOnGround(void);
	// Returns true if the player is jumping upwards
	bool isJumpUpwards(void);
	// Returns true if the player is on freefall
	bool isFreeFall(void);
	// Set the player's status to free fall mode
	void SetOnFreeFall(bool isOnFreeFall);
	// Set the player to jumping upwards
	void SetToJumpUpwards(bool isOnJumpUpwards);
	// Stop the player's vertical movement
	void StopVerticalMovement(void);
	// Reset this player instance to default
	void Reset(void);

	// Set position
	void SetPos(const Vector3& pos);
	// Set target
	void SetTarget(const Vector3& target);
	// Set Up
	void SetUp(const Vector3& up);
	// Set m_dJumpAcceleration of the player
	void SetJumpAcceleration(const double m_dJumpAcceleration);
	// Set Fall Acceleration of the player
	void SetFallAcceleration(const double m_dFallAcceleration);

	// Get position
	Vector3 GetPos(void) const;
	// Get target
	Vector3 GetTarget(void) const;
	// Get Up
	Vector3 GetUp(void) const;
	// Get Jump Acceleration of the player
	double GetJumpAcceleration(void) const;
	// Get Tile Offset for x-axis
	int GetTileOffset_x(void) const;
	// Get Max Boundary
	Vector3 GetMaxBoundary() const;
	// Get Min Boundary
	Vector3 GetMinBoundary() const;

	// Update
	void Update(double dt = 0.0333f);
	// Update tile offsets
	void UpdateTileOffset(void);
	// Update side movements
	void UpdateSideMovements(void);

	// Player movement
	void MoveUpDown(const bool mode, const float timeDiff);
	void MoveLeftRight(const bool mode, const float timeDiff);

	bool isShooting;

	// Constrain the position within the borders
	void Constrain(void);

	int checkPosition_X, checkPosition_Y;
	Vector3 position;
	int mapOffset_x, mapOffset_y;

	/*
	For Parallax Scrolling
	*/

	bool RL;
	bool chargearrow;
	float arrowSpeed;
	float arrowdmg;

	

private:
	Vector3 defaultPosition, defaultTarget, defaultUp;
	Vector3 target, up;
	Vector3 maxBoundary, minBoundary;
	int tileSize_Width, tileSize_Height;

	double m_dSpeed;
	double m_dAcceleration;

	bool m_bJumpUpwards;
	double m_dJumpSpeed;
	double m_dJumpAcceleration;

	bool m_bFallDownwards;
	double m_dFallSpeed;
	double m_dFallAcceleration;

	double m_dElapsedTime;
	double m_dElapsedDieTime;
	int playerMoveIndex;
	

	//CMap* theMapReference;

	// Codes for Scrolling
	int tileOffset_x, tileOffset_y;
	int mapFineOffset_x, mapFineOffset_y;

	// For Parallax Scrolling
	//CMap* theRearMapReference;
	int rearTileOffset_x, rearTileOffset_y;
	int rearMapOffset_x, rearMapOffset_y;
	int rearMapFineOffset_x, rearMapFineOffset_y;
};
