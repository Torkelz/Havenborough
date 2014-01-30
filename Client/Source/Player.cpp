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
	m_CurrentVelocity = Vector3(0.0f, 0.0f, 0.0f);
	m_PreviousVelocity = Vector3(0.0f, 0.0f, 0.0f);
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

	m_PreviousVelocity = m_CurrentVelocity;
	m_CurrentVelocity = m_Physics->getBodyVelocity(getBody());
}

void Player::updateAnimation(Vector3 p_Rotation)
{
	using namespace DirectX;
	Vector3 tempVector = m_CurrentVelocity;
	XMVECTOR velocity = Vector3ToXMVECTOR(&tempVector, 0.0f);
	tempVector = Vector3(m_DirectionX, 0.0f, m_DirectionZ);
	XMVECTOR look = Vector3ToXMVECTOR(&tempVector, 0.0f);
	XMMATRIX rotationInverse = XMMatrixTranspose(XMMatrixRotationRollPitchYaw(0.0f, p_Rotation.x, 0.0f));
	velocity = XMVector3Transform(velocity, rotationInverse);
	if (!m_Physics->getBodyInAir(getBody()))
	{
		// Calculate the weight on the strafe track with some trigonometry.
		float angle = XMVectorGetX(XMVector3AngleBetweenVectors(look, velocity));
		changeAnimationWeight(2, cosf(angle));

		// Decide what animation to play on the motion tracks.
		ForwardAnimationState currentForwardState = ForwardAnimationState::IDLE;
		SideAnimationState currentSideState = SideAnimationState::IDLE;

		static const float runLimit = 100.f;
		if (XMVectorGetZ(velocity) > runLimit)
		{
			currentForwardState = ForwardAnimationState::RUNNING_FORWARD;
		}
		else if (XMVectorGetZ(velocity) < -runLimit)
		{
			currentForwardState = ForwardAnimationState::RUNNING_BACKWARD;
		}
		
		static const float runSideLimit = 100.f;
		if (XMVectorGetX(velocity) > runSideLimit)
		{
			currentSideState = SideAnimationState::RUNNING_RIGHT;
		}
		else if (XMVectorGetX(velocity) < -runSideLimit)
		{
			currentSideState = SideAnimationState::RUNNING_LEFT;
		}

		if (currentForwardState != m_PrevForwardState)
		{
			switch (currentForwardState)
			{
			case ForwardAnimationState::IDLE:
				if (currentSideState == SideAnimationState::IDLE)
				{
					playAnimation("Idle", false);
				}
				break;

			case ForwardAnimationState::RUNNING_FORWARD:
				playAnimation("Run", false);
			}
		}
		
		if (currentSideState != m_PrevSideState)
		{
			switch (currentSideState)
			{
			case SideAnimationState::IDLE:
				if (currentForwardState == ForwardAnimationState::IDLE)
				{
					playAnimation("Idle", false);
				}
				break;
			}
		}

		m_PrevForwardState = currentForwardState;
		m_PrevSideState = currentSideState;
		m_PrevJumpState = JumpAnimationState::IDLE;
	}
	else
	{
		static const float runLimit = 100.f;
		float flyLimit = 500.0f;
		JumpAnimationState currentJumpState = JumpAnimationState::JUMP;

		if(XMVectorGetY(velocity) < flyLimit)
		{
			currentJumpState = JumpAnimationState::JUMP;
		}
		if(XMVectorGetY(velocity) < -flyLimit)
		{
			currentJumpState = JumpAnimationState::FALLING;
		}

		if (XMVectorGetY(velocity) > m_FallSpeed)
		{
			m_FallSpeed = XMVectorGetY(velocity);
		}

		//if (XMVectorGetY(velocity) < flyLimit && XMVectorGetY(velocity) > -flyLimit && 
		//	m_PrevJumpState != JumpAnimationState::JUMP) //SKA BERO PÅ EN BOOL SOM SÄTTS DÅ BODY SLÅR I MARKEN OAVSETT RIKTNING PÅ Y
		//{
		//	if(m_FallSpeed >= 200.0f)
		//	{
		//		currentJumpState = JumpAnimationState::HARD_LANDING;
		//	}
		//	else
		//	{
		//		currentJumpState = JumpAnimationState::LIGHT_LANDING;
		//	}
		//}

		if (currentJumpState != m_PrevJumpState)
		{
			switch (currentJumpState)
			{
			case JumpAnimationState::IDLE:
				if (currentJumpState == JumpAnimationState::IDLE)
				{
					playAnimation("Idle", false);
				}
				break;

			case JumpAnimationState::JUMP:
				if (m_PrevJumpState != JumpAnimationState::FLYING)
				{
					if(XMVectorGetX(velocity) > runLimit || XMVectorGetZ(velocity) > runLimit)
					{
						playAnimation("RunningJump", false);
						queueAnimation("Falling");
					}
					else
					{
						playAnimation("StandingJump", false);
						queueAnimation("Falling");
					}
				}
				break;

			case JumpAnimationState::FLYING:
				//playAnimation(temp, "Flying", false);
				break;

			case JumpAnimationState::HARD_LANDING:
				playAnimation("HardLanding", false);
				break;

			case JumpAnimationState::LIGHT_LANDING:
				playAnimation("NormalLanding", false);
				break;

			case JumpAnimationState::FALLING:
				//playAnimation(temp, "Falling", false);
				break;

			default: // Just in case so that the code doesn't break, hohohoho
				break;
			}
		}

		m_PrevForwardState = ForwardAnimationState::IDLE;
		m_PrevSideState = SideAnimationState::IDLE;
		m_PrevJumpState = currentJumpState;
	}
}

Vector3 Player::getVelocity() const
{
	return m_CurrentVelocity;
}

Vector3 Player::getPreviousVelocity() const
{
	return m_PreviousVelocity;
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
	XMFLOAT3 forceDiff = XMFLOAT3(force.x - m_PrevForce.x, 0.f, force.z - m_PrevForce.z);	// kg * m/s^2
	m_PrevForce = force;

	m_Physics->applyForce(getBody(), XMFLOAT3ToVector3(&forceDiff));

	m_DirectionX = m_DirectionZ = 0.f;
}

void Player::playAnimation(std::string p_AnimationName, bool p_Override)
{
	std::shared_ptr<ModelComponent> comp = m_Actor.lock()->getComponent<ModelComponent>(ModelInterface::m_ComponentId).lock();
	if (comp)
	{
		m_Actor.lock()->getEventManager()->queueEvent(IEventData::Ptr(new PlayAnimationEventData(comp->getId(), p_AnimationName, p_Override)));
	}
}

void Player::queueAnimation(std::string p_AnimationName)
{
	std::shared_ptr<ModelComponent> comp = m_Actor.lock()->getComponent<ModelComponent>(ModelInterface::m_ComponentId).lock();
	if (comp)
	{
		m_Actor.lock()->getEventManager()->queueEvent(IEventData::Ptr(new QueueAnimationEventData(comp->getId(), p_AnimationName)));
	}
}

void Player::changeAnimationWeight(int p_Track, float p_Weight)
{
	std::shared_ptr<ModelComponent> comp = m_Actor.lock()->getComponent<ModelComponent>(ModelInterface::m_ComponentId).lock();
	if (comp)
	{
		m_Actor.lock()->getEventManager()->queueEvent(IEventData::Ptr(new ChangeAnimationWeightEventData(comp->getId(), p_Track, p_Weight)));
	}
}