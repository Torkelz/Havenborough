#include "Player.h"
#include "Components.h"

using namespace DirectX;

Player::Player(void)
{
	m_Physics = nullptr;
	m_JumpCount = 0;
    m_JumpCountMax = 2;
    m_JumpDelay = 0;
    m_JumpDelayMax = 0.1f;
    m_JumpTime = 0.f;
    m_JumpTimeMax = 0.15f;
	m_JumpForce = 6500.f;
	m_IsJumping = false;
	m_PrevForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_MaxSpeed = 1000.f;
	m_AccConstant = 250.f;
	m_DirectionZ = 0.f;
	m_DirectionX = 0.f;
	m_ForceMove = false;
	m_ForceMoveTime = 1.0f;
	m_ForceMoveSpeed = 1.f;
	m_CurrentForceMoveTime = 0.f;
	m_FallSpeed = 0.0f;
}

Player::~Player(void)
{
	m_Physics = nullptr;
}

void Player::initialize(IPhysics *p_Physics, XMFLOAT3 p_LookDirection, std::weak_ptr<Actor> p_Actor)
{
	m_Physics = p_Physics;
	m_LookDirection = p_LookDirection;
	m_TempHeight = 180.f;
	m_KneeHeight = 50.f;
	m_EyeHeight = 165.f;
	m_Actor = p_Actor;
}

XMFLOAT3 Player::getPosition(void) const
{
	Vector3 pos = getCollisionCenter();
	pos.y -= m_KneeHeight;
	return pos;
}

XMFLOAT3 Player::getEyePosition() const
{
	Actor::ptr actor = m_Actor.lock();
	if (actor)
	{
		std::shared_ptr<AnimationInterface> comp = actor->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId).lock();
		if (comp)
		{
			//return comp->getJointPos("HeadBase");
		}
	}

	XMFLOAT3 eyePosition = getPosition();
	eyePosition.y += m_EyeHeight;
	return eyePosition;
}

void Player::setPosition(const XMFLOAT3 &p_Position)
{
	Vector3 kneePos = p_Position;
	kneePos.y += m_KneeHeight;
	m_Physics->setBodyPosition(getBody(), kneePos);
}


XMFLOAT3 Player::getGroundPosition() const
{
	return getPosition();
}

XMFLOAT3 Player::getCollisionCenter() const
{
	if (m_Actor.expired())
	{
		return Vector3(0.f, 0.f, 0.f);
	}

	return m_Physics->getBodyPosition(getBody());
}

float Player::getHeight() const
{
	return m_TempHeight;
}

BodyHandle Player::getBody(void) const
{
	return m_Actor.lock()->getBodyHandles()[0];
}

void Player::setJump(void)
{
	if(m_Physics->getBodyInAir(getBody()))
	{
		m_JumpCount++;
	}

	if(!m_IsJumping && m_JumpCount < m_JumpCountMax)
	{
		//m_JumpCount++;
		m_IsJumping = true;
		if(m_JumpCount > 0)
		{
			Vector3 temp = m_Physics->getBodyVelocity(getBody());
			m_Physics->setBodyVelocity(getBody(), Vector3(temp.x, 0.f, temp.z));
		}

		m_Physics->applyForce(getBody(), Vector3(0.f, m_JumpForce, 0.f));
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

void Player::forceMove(Vector3 p_StartPosition, Vector3 p_EndPosition)
{
	if(!m_ForceMove)
	{
		m_ForceMove = true;
		m_ForceMoveStartPosition = p_StartPosition;
		m_ForceMoveEndPosition = p_EndPosition;
		m_Physics->setBodyVelocity(getBody(), Vector3(0,0,0));
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

		XMVECTOR startPos = XMLoadFloat3(&((XMFLOAT3)m_ForceMoveStartPosition));
		XMVECTOR endPos = XMLoadFloat3(&((XMFLOAT3)m_ForceMoveEndPosition));

		XMVECTOR currPosition = XMVectorLerp(startPos,
			endPos, dt);
		XMFLOAT3 newGroundPosition;
		XMStoreFloat3(&newGroundPosition, currPosition);
		setPosition(newGroundPosition);

		m_CurrentForceMoveTime += p_DeltaTime * m_ForceMoveSpeed;
		if(m_CurrentForceMoveTime > m_ForceMoveTime)
		{
			m_ForceMove = false;
			m_CurrentForceMoveTime = 0.f;
		}
	}
}

Vector3 Player::getVelocity() const
{
	return m_Physics->getBodyVelocity(getBody());
}

Vector3 Player::getDirection() const
{
	return Vector3(m_DirectionX, 0.0f, m_DirectionZ);
}

std::weak_ptr<Actor> Player::getActor() const
{
	return m_Actor;
}

void Player::setActor(std::weak_ptr<Actor> p_Actor)
{
	m_Actor = p_Actor;
}

void Player::jump(float dt)
{
	if(m_IsJumping)
	{
		m_JumpTime += dt;
		if(m_JumpTime > m_JumpTimeMax)
		{
			m_Physics->applyForce(getBody(), Vector3(0.f, -m_JumpForce, 0.f));
			m_IsJumping = false;
			m_JumpTime = 0.f;
		}
	}
	if(!m_IsJumping && !m_Physics->getBodyInAir(getBody()))
    {
		m_JumpCount = 0;
    }

	if(m_Physics->getBodyLanded(getBody()))
	{
		//m_JumpCount = 0;
	}
}

void Player::move()
{
	Vector3 velocity = m_Physics->getBodyVelocity(getBody());
	XMFLOAT3 currentVelocity = velocity;	// cm/s
	currentVelocity.y = 0.f;

	XMFLOAT3 maxVelocity(m_DirectionX * m_MaxSpeed, 0.f, m_DirectionZ * m_MaxSpeed);	// cm/s

	XMFLOAT3 diffVel = XMFLOAT3(0.f, 0.f, 0.f);	// cm/s
	XMFLOAT3 force = XMFLOAT3(0.f, 0.f, 0.f);		// kg * m/s^2

	diffVel.x = maxVelocity.x - currentVelocity.x;
	diffVel.y = maxVelocity.y - currentVelocity.y;
	diffVel.z = maxVelocity.z - currentVelocity.z;

	force.x = diffVel.x / 100.f * m_AccConstant;
	force.y = diffVel.y / 100.f * m_AccConstant;
	force.z = diffVel.z / 100.f * m_AccConstant;

	Vector4 direction = m_Physics->calculateDirectionVector(getBody(), force);

	force = direction.xyz();

	XMFLOAT3 forceDiff = XMFLOAT3(force.x - m_PrevForce.x, force.y - m_PrevForce.y, force.z - m_PrevForce.z);	// kg * m/s^2
	m_PrevForce = force;

	m_Physics->applyForce(getBody(), XMFLOAT3ToVector3(&forceDiff));

	m_DirectionX = m_DirectionZ = 0.f;
}
