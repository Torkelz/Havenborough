#include "Player.h"

Player::Player(void)
{
	m_Physics = nullptr;
	m_JumpTime = 0.f;
	m_JumpForceTime = 0.2f;
	m_JumpForce = 2000.f;
	m_IsJumping = false;
	m_PrevForce = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_MaxSpeed = 10.f;
	m_AccConstant = 250.f;
	m_DirectionZ = 0.f;
	m_DirectionX = 0.f;
	m_ForceMove = false;
	m_ForceMoveTime = 1.0f;
	m_ForceMoveSpeed = 0.2f;
	m_CurrentForceMoveTime = 0.f;
}

Player::~Player(void)
{
	m_Physics = nullptr;
}

void Player::initialize(IPhysics *p_Physics, XMFLOAT3 p_Position, XMFLOAT3 p_LookDirection)
{
	m_Physics = p_Physics;
	m_Position = p_Position;
	m_LookDirection = p_LookDirection;
	m_TempHeight = 3.2f;
	m_PlayerBody = m_Physics->createSphere(50.f, false, XMFLOAT3ToVector3(&m_Position), m_TempHeight * 0.5f);

}

XMFLOAT3 Player::getPosition(void) const
{
	return m_Position;
}

float Player::getHeight() const
{
	return m_TempHeight;
}

BodyHandle Player::getBody(void) const
{
	return m_PlayerBody;
}

void Player::setJump(void)
{
	if(!m_IsJumping)
	{
		m_IsJumping = true;
		m_Physics->applyForce(Vector4(0.f, m_JumpForce, 0.f, 0.f), m_PlayerBody);
	}
}

void Player::setDirectionX(float p_DirectionX)
{
	m_DirectionX = p_DirectionX;
}

void Player::setDirectionZ(float p_DirectionZ)
{
	m_DirectionZ = p_DirectionZ;
}

bool Player::getForceMove(void)
{
	return m_ForceMove;
}

void Player::forceMove(XMVECTOR p_StartPosition, XMVECTOR p_EndPosition)
{
	if(!m_ForceMove)
	{
		m_ForceMove = true;
		m_ForceMoveStartPosition = p_StartPosition;
		m_ForceMoveEndPosition = p_EndPosition;
	}
}

void Player::update(float p_DeltaTime)
{
	if(!m_ForceMove)
	{
		jump(p_DeltaTime);
		move();
	}
	else
	{
		float dt = m_CurrentForceMoveTime / m_ForceMoveTime;

		XMVECTOR currPosition = XMVectorLerp(m_ForceMoveStartPosition,
			m_ForceMoveEndPosition, dt);
		XMStoreFloat3(&m_Position, currPosition);

		m_Physics->setBodyPosition(XMFLOAT3ToVector3(&m_Position), m_PlayerBody);


		m_CurrentForceMoveTime += p_DeltaTime * m_ForceMoveSpeed;
		if(m_CurrentForceMoveTime > m_ForceMoveTime)
		{
			m_ForceMove = false;
			m_CurrentForceMoveTime = 0.f;
		}
	}
}

void Player::jump(float dt)
{
	if(m_IsJumping)
	{
		m_JumpTime += dt;
		if(m_JumpTime > m_JumpForceTime)
		{
			m_Physics->applyForce(Vector4(0.f, -m_JumpForce, 0.f, 0.f), m_PlayerBody);
			m_IsJumping = false;
			m_JumpTime = 0.f;
		}
	}
}

void Player::move()
{
	Vector4 velocity = m_Physics->getVelocity(m_PlayerBody);
	XMFLOAT4 currentVelocity = Vector4ToXMFLOAT4(&velocity);
	currentVelocity.y = 0.f;
	XMFLOAT4 maxVelocity(-m_DirectionX * m_MaxSpeed, 0.f, -m_DirectionZ * m_MaxSpeed, 0.f);

	XMFLOAT4 diffVel = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4 force = XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	diffVel.x = maxVelocity.x - currentVelocity.x;
	diffVel.y = maxVelocity.y - currentVelocity.y;
	diffVel.z = maxVelocity.z - currentVelocity.z;
	diffVel.w = 0.f;

	force.x = diffVel.x * m_AccConstant;
	force.y = diffVel.y * m_AccConstant;
	force.z = diffVel.z * m_AccConstant;
	force.w = 0.f;
	XMFLOAT4 forceDiff = XMFLOAT4(force.x - m_PrevForce.x, 0.f, force.z - m_PrevForce.z, 0.f); 
	m_PrevForce = force;

	m_Physics->applyForce(XMFLOAT4ToVector4(&forceDiff), m_PlayerBody);

	m_DirectionX = m_DirectionZ = 0.f;

	Vector4 position = m_Physics->getBodyPosition(m_PlayerBody);
	m_Position = Vector4ToXMFLOAT3(&position);
}