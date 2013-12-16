#include "Player.h"

Player::Player(void)
{
	m_Physics = nullptr;
	m_JumpTime = 0.f;
	m_JumpForceTime = 0.2f;
	m_JumpForce = 2000.f;
	m_IsJumping = false;
	m_PrevForce = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	maxSpeed = 10.f;
	accConstant = 250.f;
	m_DirectionZ = 0.f;
	m_DirectionX = 0.f;
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
	m_PlayerBody = m_Physics->createSphere(50.f, false, XMFLOAT3ToVector3(&m_Position), 1.6f);

}

XMFLOAT3 Player::getPosition(void) const
{
	return m_Position;
}

BodyHandle Player::getBody() const
{
	return m_PlayerBody;
}

void Player::setJump()
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

void Player::update(float dt)
{
	jump(dt);

	move();
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
	XMFLOAT4 currentVelocity = Vector4ToXMFLOAT4(&(m_Physics->getVelocity(m_PlayerBody)));
	currentVelocity.y = 0.f;
	XMFLOAT4 maxVelocity(-m_DirectionX * maxSpeed, 0.f, -m_DirectionZ * maxSpeed, 0.f);

	XMFLOAT4 diffVel = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4 force = XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	diffVel.x = maxVelocity.x - currentVelocity.x;
	diffVel.y = maxVelocity.y - currentVelocity.y;
	diffVel.z = maxVelocity.z - currentVelocity.z;
	diffVel.w = 0.f;

	force.x = diffVel.x * accConstant;
	force.y = diffVel.y * accConstant;
	force.z = diffVel.z * accConstant;
	force.w = 0.f;
	XMFLOAT4 forceDiff = XMFLOAT4(force.x - m_PrevForce.x, 0.f, force.z - m_PrevForce.z, 0.f); 
	m_PrevForce = force;

	m_Physics->applyForce(XMFLOAT4ToVector4(&forceDiff), m_PlayerBody);

	m_DirectionX = m_DirectionZ = 0.f;
}