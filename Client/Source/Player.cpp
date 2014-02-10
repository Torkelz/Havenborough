#include "Player.h"
#include "Components.h"

using namespace DirectX;

Player::Player(void)
{
	m_Physics = nullptr;
	m_JumpCount = 0;
    m_JumpCountMax = 2;
    m_JumpDelay = 0.f;
    m_JumpDelayMax = 0.15f;
    m_JumpTime = 0.f;
    m_JumpTimeMax = 0.15f;
	m_JumpForce = 6500.f;
	m_IsJumping = false;
	m_MaxSpeed = 1000.f;
	m_AccConstant = 600.f;
	m_DirectionZ = 0.f;
	m_DirectionX = 0.f;
	m_ForceMove = false;
	m_ForceMoveTime = 1.0f;
	m_ForceMoveSpeed = 1.f;
	m_CurrentForceMoveTime = 0.f;
	m_GroundNormal = XMFLOAT3(0.f, 1.f, 0.f);
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
	//m_PlayerBody = m_Physics->createOBB(68.f, false, kneePos, Vector3(50.f, 90.f, 50.f), false);
	//m_PlayerBody = m_Physics->createSphere(68.f, false, kneePos, m_KneeHeight);
}

XMFLOAT3 Player::getPosition(void) const
{
	Actor::ptr actor = m_Actor.lock();
	if (actor)
	{
		return actor->getPosition();
	}

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
			return comp->getJointPos("Hip");
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

float Player::getChestHeight() const
{
	return m_TempHeight * 0.75f;;
}

float Player::getWaistHeight() const
{
	return m_TempHeight * 0.5f;;
}

float Player::getKneeHeight() const
{
	return m_TempHeight * 0.25f;
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

		Vector3 temp = m_Physics->getBodyVelocity(getBody());
		temp.y = 0.f;

		temp.x = m_DirectionX * m_MaxSpeed / (m_JumpCount + 1);
		temp.z = m_DirectionZ * m_MaxSpeed / (m_JumpCount + 1);

		m_Physics->setBodyVelocity(getBody(), temp);

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

void Player::forceMove(std::string p_ClimbId, DirectX::XMFLOAT3 p_CollisionNormal)
{
	if(!m_ForceMove)
	{
		XMVECTOR fwd = XMVectorSet(p_CollisionNormal.x, 0.f,p_CollisionNormal.z,0);
		XMVECTOR len = XMVector3Length(fwd);
		if (XMVectorGetX(len) == 0.f)
		{
			return;
		}
		fwd /= len;

		m_ForceMove = true;
		m_Physics->setBodyVelocity(getBody(), Vector3(0,0,0));
		std::weak_ptr<AnimationInterface> aa = m_Actor.lock()->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId);
		AnimationPath pp = aa.lock()->getAnimationData(p_ClimbId);
		aa.lock()->playClimbAnimation(p_ClimbId);

		m_ForceMoveY = pp.m_YPath;
		m_ForceMoveZ = pp.m_ZPath;
		//m_ForceMoveNormal = p_CollisionNormal;
		m_ForceMoveStartPos = getPosition();
		
		fwd *= -1.f;
		XMVECTOR up = XMVectorSet(0,1,0,0);
		XMVECTOR side = XMVector3Normalize(XMVector3Cross(up, fwd));
		//up = XMVector3Normalize(XMVector3Cross(side, fwd));

		//up *= -1.0f;
		
		XMMATRIX a;
		a.r[0] = side;
		a.r[1] = up;
		a.r[2] = fwd;
		a.r[3] = XMVectorSet(0,0,0,1);
		XMStoreFloat4x4(&m_ForceMoveRotation, a);
	}
}

void Player::update(float p_DeltaTime)
{
	if(!m_ForceMove)
	{
		jump(p_DeltaTime);

		if (!m_Physics->getBodyInAir(getBody()) || m_IsJumping)
		{
			move(p_DeltaTime);
		}
	}
	else
	{
		unsigned int currentFrame = (unsigned int)m_CurrentForceMoveTime;

		// Check if you have passed the goal frame.
		if(currentFrame >= m_ForceMoveY[1].y)
			m_ForceMoveY.erase(m_ForceMoveY.begin());
		if(currentFrame >= m_ForceMoveZ[1].y)
			m_ForceMoveZ.erase(m_ForceMoveZ.begin());

		// Check if you only have one frame left.
		if(m_ForceMoveY.size() < 2 && m_ForceMoveZ.size() < 2)
		{
			m_ForceMove = false;
			m_CurrentForceMoveTime = 0.f;
			std::weak_ptr<AnimationInterface> aa = m_Actor.lock()->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId);
			aa.lock()->resetClimbState();
			return;
		}

		float currentFrameTime = (m_CurrentForceMoveTime - (float)m_ForceMoveY[0].y);
		float currentFrameSpan = (float)(m_ForceMoveY[1].y - m_ForceMoveY[0].y);
		float timeFrac = currentFrameTime / currentFrameSpan;
		float currentYPos = m_ForceMoveY[0].x + ((m_ForceMoveY[1].x - m_ForceMoveY[0].x) * timeFrac);

		currentFrameTime = (m_CurrentForceMoveTime - (float)m_ForceMoveZ[0].y);
		currentFrameSpan = (float)(m_ForceMoveZ[1].y - m_ForceMoveZ[0].y);
		timeFrac = currentFrameTime / currentFrameSpan;
		float currentZPos = m_ForceMoveZ[0].x + ((m_ForceMoveZ[1].x - m_ForceMoveZ[0].x) * timeFrac);

		m_CurrentForceMoveTime += p_DeltaTime * 24.0f; // 24 FPS

		DirectX::XMFLOAT3 temp;
		DirectX::XMVECTOR tv = DirectX::XMVectorSet(0,currentYPos,currentZPos,0);
		DirectX::XMVECTOR tstart = DirectX::XMLoadFloat3(&m_ForceMoveStartPos);
		XMMATRIX rotation = XMLoadFloat4x4(&m_ForceMoveRotation);

		tv = XMVector3Transform(tv, rotation);

		DirectX::XMStoreFloat3(&temp, tstart+tv);
		setPosition(temp);
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

DirectX::XMFLOAT3 Player::getGroundNormal() const
{
	return m_GroundNormal;
}

void Player::setGroundNormal(DirectX::XMFLOAT3 p_Normal)
{
	m_GroundNormal = p_Normal;
}

void Player::jump(float dt)
{
	if(m_IsJumping)
	{
		m_JumpTime += dt;
		m_JumpDelay += dt;
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
		//m_JumpDelay = 0.f;
    }

	if(m_Physics->getBodyLanded(getBody()))
	{
		//m_JumpCount = 0;
	}
}

void Player::move(float p_DeltaTime)
{
	XMFLOAT3 velocity = m_Physics->getBodyVelocity(getBody());
	XMVECTOR currentVelocity = XMLoadFloat3(&velocity);	// cm/s

	XMFLOAT3 maxVelocity(m_DirectionX * m_MaxSpeed, 0.f, m_DirectionZ * m_MaxSpeed);	// cm/s
	if (m_IsJumping)
	{
		maxVelocity.y = m_MaxSpeed*0.5f;
	}
	XMVECTOR vMaxVelocity = XMLoadFloat3(&maxVelocity);
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR groundNormal = XMLoadFloat3(&m_GroundNormal);
	XMVECTOR rotAxis = XMVector3Cross(groundNormal, up);
	if (XMVector3Dot(rotAxis, rotAxis).m128_f32[0] > 0.001f)
	{
		float angle = XMVector3AngleBetweenVectors(groundNormal, up).m128_f32[0];

		if (angle < PI * 0.3f)
		{
			XMMATRIX rotMatrix = XMMatrixRotationAxis(rotAxis, -angle);
			vMaxVelocity = XMVector3Transform(vMaxVelocity, rotMatrix);
		}
	}

	XMVECTOR diffVel = vMaxVelocity - currentVelocity;	// cm/s
	XMVECTOR force = diffVel / 100.f * m_AccConstant;		// kg * m/s^2

	XMVECTOR forceDiffImpulse = force * p_DeltaTime;	// kg * m/s

	XMFLOAT3 fForceDiff;
	XMStoreFloat3(&fForceDiff, forceDiffImpulse);

	m_Physics->applyImpulse(getBody(), fForceDiff);

	m_DirectionX = m_DirectionZ = 0.f;
}
