#include "Animation.h"


CAnimation::CAnimation()
	: m_bAnimationInvert(false)
	, isMoving(false)
	, m_iAnimation_Index(4)
	, m_iRight_Start(0)
	, m_iRight_End(0)
	, m_iLeft_Start(0)
	, m_iLeft_End(0)
	, m_iRightIdle_Start(0)
	, m_iRightIdle_End(0)
	, m_iLeftIdle_Start(0)
	, m_iLeftIdle_End(0)
	, m_dElapsedTime(0.0f)
{
}


CAnimation::~CAnimation()
{
}

// Set Animation status; left or right
void CAnimation::SetAnimationStatus(bool m_bAnimationInvert,bool isMoving, bool isAttacking, double dt)
{
	if (this->m_bAnimationInvert != m_bAnimationInvert || this->isMoving != isMoving || this->isAttacking != isAttacking)
	{
		this->m_bAnimationInvert = m_bAnimationInvert;
		this->isMoving = isMoving;

		if (!m_bAnimationInvert && !isMoving)
		{
			m_iAnimation_Index = m_iRightIdle_Start;
		}
		else if (m_bAnimationInvert && !isMoving)
		{
			m_iAnimation_Index = m_iLeftIdle_Start;
		}
		else if (!m_bAnimationInvert && isMoving)
		{
			m_iAnimation_Index = m_iRight_Start;
		}
		else if (m_bAnimationInvert && isMoving)
		{
			m_iAnimation_Index = m_iLeft_Start;
		}
	}
	UpdateAnimationIndex(dt);
}
// Update the Animation Index
void CAnimation::UpdateAnimationIndex(double dt)
{
	m_dElapsedTime += dt;

	if (!m_bAnimationInvert && isMoving)
	{
		if (m_dElapsedTime > 0.3f)
		{
			m_iAnimation_Index += 1;
			// If the player is facing right
			if (m_iAnimation_Index > m_iRight_End)
				m_iAnimation_Index = m_iRight_Start;
			m_dElapsedTime = 0.f;
		}
	}
	else if(m_bAnimationInvert && isMoving)
	{
		if (m_dElapsedTime > 0.3f)
		{
			m_iAnimation_Index -= 1;
			// If the player is facing left
			if (m_iAnimation_Index < m_iLeft_Start)
				m_iAnimation_Index = m_iLeft_End;
			m_dElapsedTime = 0.f;
		}
	}
	else if (!m_bAnimationInvert && !isMoving)
	{
		if (m_dElapsedTime > 0.3f)
		{
			m_iAnimation_Index += 1;
			// If the player is facing right
			if (m_iAnimation_Index > m_iRightIdle_End)
				m_iAnimation_Index = m_iRightIdle_Start;
			m_dElapsedTime = 0.f;
		}
	}
	else if (m_bAnimationInvert && !isMoving)
	{
		if (m_dElapsedTime > 0.3f)
		{
			m_iAnimation_Index -= 1;
			// If the player is facing right
			if (m_iAnimation_Index < m_iLeftIdle_Start)
				m_iAnimation_Index = m_iLeftIdle_End;
			m_dElapsedTime = 0.f;
		}
	}
}
// Get the Animation status
bool CAnimation::GetAnimationStatus(void) const
{
	return m_bAnimationInvert;
}
// Get the Animation index
int CAnimation::GetAnimationIndex(void) const
{
	return m_iAnimation_Index;
}

// Set right indices
void CAnimation::SetRightIndices(const int m_iRight_Start, const int m_iRight_End)
{
	this->m_iRight_Start = m_iRight_Start;
	this->m_iRight_End = m_iRight_End;
}

// Set left indices
void CAnimation::SetLeftIndices(const int m_iLeft_Start, const int m_iLeft_End)
{
	this->m_iLeft_Start = m_iLeft_Start;
	this->m_iLeft_End = m_iLeft_End;
}

void CAnimation::SetRightIdleIndices(const int m_iRightIdle_Start, const int m_iRightIdle_End)
{
	this->m_iRightIdle_Start = m_iRightIdle_Start;
	this->m_iRightIdle_End = m_iRightIdle_End;
}

void CAnimation::SetLeftIdleIndices(const int m_iLeftIdle_Start, const int m_iLeftIdle_End)
{
	this->m_iLeftIdle_Start = m_iLeftIdle_Start;
	this->m_iLeftIdle_End = m_iLeftIdle_End;
}

void CAnimation::SetRightAttIndices(const int m_iRightAtt_Start, const int m_iRightAtt_End)
{
}

void CAnimation::SetLeftAttIndices(const int m_iLeftAtt_Start, const int m_iLeftAtt_End)
{
}
