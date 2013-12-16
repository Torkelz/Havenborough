#include "Player.h"

Player::Player(void)
{
	m_Physics = nullptr;
	m_JumpTime = 0.f;
	m_JumpForceTime = 0.2f;
	m_JumpForce = 2000.f;
	m_IsJumping = false;
	m_PrevForce = Vector4(0,0,0,0);
	maxSpeed = 10.f;
	accConstant = 250.f;
	m_DirectionZ = 0.f;
	m_DirectionX = 0.f;
}


Player::~Player(void)
{
	m_Physics = nullptr;
}

void Player::initialize(IPhysics *p_Physics, Vector3 p_Position, Vector3 p_LookDirection)
{
	m_Physics = p_Physics;
	m_Position = p_Position;
	m_LookDirection = p_LookDirection;
	//float l[] = {m_Position.x,m_Position.y,m_Position.z};

	//float d[3] = XMFLOAT3ToFloat3(l);
	m_PlayerBody = m_Physics->createSphere(50.f, false, m_Position, 1.6f);

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
	Vector4 currentVelocity = m_Physics->getVelocity(m_PlayerBody);
	currentVelocity.y = 0.f;
	Vector4 maxVelocity(-m_DirectionX * maxSpeed, 0.f, -m_DirectionZ * maxSpeed, 0.f);

	Vector4 diffVel = Vector4(0.f, 0.f, 0.f, 0.f);
	Vector4 force = Vector4(0.f, 0.f, 0.f, 0.f);

	diffVel.x = maxVelocity.x - currentVelocity.x;
	diffVel.y = maxVelocity.y - currentVelocity.y;
	diffVel.z = maxVelocity.z - currentVelocity.z;
	diffVel.w = 0.f;

	force.x = diffVel.x * accConstant;
	force.y = diffVel.y * accConstant;
	force.z = diffVel.z * accConstant;
	force.w = 0.f;

	Vector4 forceDiff = Vector4(force.x - m_PrevForce.x, 0.f, force.z - m_PrevForce.z, 0.f); 
	m_PrevForce = force;

	m_Physics->applyForce(forceDiff, m_PlayerBody);

	m_DirectionX = m_DirectionZ = 0.f;
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