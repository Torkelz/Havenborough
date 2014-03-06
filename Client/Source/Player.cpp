#include "Player.h"
#include "Components.h"
#include "RunControlComponent.h"
#include <Logger.h>

using namespace DirectX;

Player::Player(void)
{
	m_Physics = nullptr;
	m_JumpCount = 0;
    m_JumpCountMax = 2;
    m_JumpTime = 0.f;
    m_JumpTimeMax = 0.2f;
	m_JumpForce = 7000.f;
	m_ForceMove = false;
	m_CurrentForceMoveTime = 0.f;
	m_Height = 170.f;
	m_EyeHeight = 165.f;
	m_Climb = false;
	m_ClimbOffset = 0.f;
	m_MaxMana = 100.f;
	m_CurrentMana = m_MaxMana;
	m_PreviousMana = m_CurrentMana;
	m_ManaRegenerationSlow = 3.f;
	m_ManaRegenerationFast = 10.f;
	m_IsAtMaxSpeed = false;
	m_IsPreviousManaSet = false;
	m_AllowedToMove = true;
	m_ClimbSpeedUp = 1.0f;
}

Player::~Player(void)
{
	m_Physics = nullptr;
}

void Player::initialize(IPhysics *p_Physics, INetwork *p_Network, std::weak_ptr<Actor> p_Actor)
{
	m_Physics = p_Physics;
	m_Network = p_Network;
	m_Actor = p_Actor;

	setCurrentMana(0.f);

	Actor::ptr strActor = m_Actor.lock();
	if (strActor)
	{
		m_LastSafePosition = strActor->getPosition();
	}
}

void Player::update(float p_DeltaTime)
{	
	if(!m_IsPreviousManaSet)
		m_PreviousMana = m_CurrentMana;
	else
		m_IsPreviousManaSet = false;

	Vector3 v3Vel = m_Physics->getBodyVelocity(getBody());
	float v = XMVector4Length(Vector3ToXMVECTOR(&v3Vel, 0.f)).m128_f32[0];
	std::shared_ptr<MovementControlInterface> moveComp = m_Actor.lock()->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
	if(moveComp && v >= moveComp->getMaxSpeedDefault() - 100.f)
	{
		m_IsAtMaxSpeed = true;
		m_CurrentMana += m_ManaRegenerationFast * p_DeltaTime;
	}
	else
	{
		m_IsAtMaxSpeed = false;
		m_CurrentMana += m_ManaRegenerationSlow * p_DeltaTime;
	}

	if(m_CurrentMana >= m_MaxMana)
		m_CurrentMana = m_MaxMana;

	static const float respawnFallHeight = -2000.f; // -20m
	static const float respawnDistance = 100000.f; // 1000m
	static const float respawnDistanceSq = respawnDistance * respawnDistance;

	Actor::ptr strActor = m_Actor.lock();
	if (strActor)
	{
		Vector3 currentPos = strActor->getPosition();
		const float distanceSq =
			currentPos.x * currentPos.x +
			currentPos.y * currentPos.y +
			currentPos.z * currentPos.z;

		if (currentPos.y < respawnFallHeight
			|| distanceSq > respawnDistanceSq)
		{
			strActor->setPosition(m_LastSafePosition);

			std::shared_ptr<PhysicsInterface> comp = strActor->getComponent<PhysicsInterface>(PhysicsInterface::m_ComponentId).lock();
			if (comp)
			{
				m_Physics->setBodyVelocity(comp->getBodyHandle(), Vector3(0.f, 0.f, 0.f));
			}
		}

		strActor->getEventManager()->queueEvent(IEventData::Ptr(new UpdateGraphicalManabarEventData( m_CurrentMana/100, m_PreviousMana/100)));
	}

	if(!m_ForceMove)
	{
		if(m_AllowedToMove)
		{
			jump(p_DeltaTime);
			
			if (strActor)
			{
				std::shared_ptr<MovementControlInterface> comp = strActor->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
				if (comp)
				{
					comp->move(p_DeltaTime);
				}
			}
		}
		else
		{
			if(strActor)
			{
				std::shared_ptr<PhysicsInterface> comp = strActor->getComponent<PhysicsInterface>(PhysicsInterface::m_ComponentId).lock();
				Vector3 fel = m_Physics->getBodyVelocity(comp->getBodyHandle());
				m_Physics->setBodyVelocity(comp->getBodyHandle(), Vector3(0.f, fel.y, 0.f));
			}
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
			if (m_Network)
			{
				IConnectionController* con = m_Network->getConnectionToServer();
				if (con && con->isConnected())
				{
					tinyxml2::XMLPrinter printer;
					printer.OpenElement("Action");
					printer.OpenElement("ResetClimb");
					printer.CloseElement();
					printer.CloseElement();

					con->sendObjectAction(m_Actor.lock()->getId(), printer.CStr());
				}
			}
			return;
		}

		float currentFrameTime = m_CurrentForceMoveTime - m_ForceMoveY[0].y;
		float currentFrameSpan = m_ForceMoveY[1].y - m_ForceMoveY[0].y;
		float timeFrac = currentFrameTime / currentFrameSpan;
		float currentYPos = m_ForceMoveY[0].x + ((m_ForceMoveY[1].x - m_ForceMoveY[0].x) * timeFrac);

		currentFrameTime = m_CurrentForceMoveTime - m_ForceMoveZ[0].y;
		currentFrameSpan = m_ForceMoveZ[1].y - m_ForceMoveZ[0].y;
		timeFrac = currentFrameTime / currentFrameSpan;
		float currentZPos = m_ForceMoveZ[0].x + ((m_ForceMoveZ[1].x - m_ForceMoveZ[0].x) * timeFrac);

		m_CurrentForceMoveTime += p_DeltaTime * 24.0f * m_ClimbSpeedUp; // 24 FPS

		DirectX::XMFLOAT3 temp;
		DirectX::XMVECTOR tv = DirectX::XMVectorSet(0,currentYPos,currentZPos,0);
		DirectX::XMVECTOR tstart = DirectX::XMLoadFloat3(&m_ForceMoveStartPos);
		XMMATRIX rotation = XMLoadFloat4x4(&m_ForceMoveRotation);

		tv = XMVector3Transform(tv, rotation);

		DirectX::XMStoreFloat3(&temp, tstart+tv);
		setPosition(temp);

		std::shared_ptr<LookInterface> look = m_Actor.lock()->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
		if (look)
		{
			look->setLookForward(Vector3(m_forward.x, m_forward.y, m_forward.z));
			look->setLookUp(Vector3(0, 1, 0));
		}
	}
}

void Player::forceMove(std::string p_ClimbId, DirectX::XMFLOAT3 p_CollisionNormal, DirectX::XMFLOAT3 p_BoxPos, DirectX::XMFLOAT3 p_EdgeOrientation)
{
	if(!m_ForceMove && m_Climb)
	{
		XMVECTOR fwd = XMVectorSet(p_CollisionNormal.x, 0.f,p_CollisionNormal.z,0);
		XMVECTOR len = XMVector3Length(fwd);
		if (XMVectorGetX(len) == 0.f)
		{
			return;
		}
		fwd /= len;

		m_ForceMove = true;
		
		//if(p_ClimbId == "Climb3" || p_ClimbId == "Climb4")
		//{
			m_Physics->resetForceOnBody(getBody());
			
			Actor::ptr actor = m_Actor.lock();
			if (actor)
			{
				std::shared_ptr<MovementControlInterface> comp = actor->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
				std::shared_ptr<RunControlComponent> runComp = std::dynamic_pointer_cast<RunControlComponent>(comp);

				if (runComp)
				{
					runComp->setIsJumping(false);
				}
			}
			m_JumpCount = 0;
		//}
		//m_Physics->setBodyVelocity(getBody(), Vector3(0,0,0));
		std::weak_ptr<AnimationInterface> aa = m_Actor.lock()->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId);
		AnimationPath pp = aa.lock()->getAnimationData(p_ClimbId);
		aa.lock()->playClimbAnimation(p_ClimbId);
		m_ClimbId = p_ClimbId;

		m_ForceMoveY = pp.m_YPath;
		m_ForceMoveZ = pp.m_ZPath;
		m_ClimbSpeedUp = pp.m_Speed;
		m_ForceMoveStartPos = getPosition();

		fwd *= -1.f;
		XMVECTOR up = XMVectorSet(0,1,0,0);
		XMVECTOR side = XMVector3Normalize(XMVector3Cross(up, fwd));
		
		XMMATRIX a;
		a.r[0] = side;
		a.r[1] = up;
		a.r[2] = fwd;
		a.r[3] = XMVectorSet(0,0,0,1);
		XMStoreFloat4x4(&m_ForceMoveRotation, a);

		float edgeY = p_EdgeOrientation.y;
		XMStoreFloat3(&p_EdgeOrientation, XMVector3Normalize(XMVectorSet(p_EdgeOrientation.x, p_EdgeOrientation.y, p_EdgeOrientation.z, 0)));

		XMFLOAT3 playerOrigPos = getPosition();
		XMVECTOR vReachPointCenter = XMLoadFloat3(&playerOrigPos) - XMLoadFloat3(&p_BoxPos);
		//XMVECTOR vReachPointCenter = Vector3ToXMVECTOR(&m_Physics->getBodyPosition(getBody()), 0.f) - XMLoadFloat3(&p_BoxPos);
		
		// Check if the orientation needs to be flipped to avoid that the arms cross during IK calcs.
		if(!(p_EdgeOrientation.x * side.m128_f32[0] >= 0.f && p_EdgeOrientation.z * side.m128_f32[2] >= 0.f))
			p_EdgeOrientation = XMFLOAT3(-p_EdgeOrientation.x, -p_EdgeOrientation.y, -p_EdgeOrientation.z);

		XMVECTOR vEdgeOrientation = XMLoadFloat3(&p_EdgeOrientation);

		// The goldener path code IS A LIE!
		//vReachPointCenter = (XMVector3Dot(vReachPointCenter, vEdgeOrientation) * vEdgeOrientation) + XMLoadFloat3(&p_BoxPos);
		//XMStoreFloat3(&m_CenterReachPos, vReachPointCenter);
		//XMStoreFloat3(&m_Side, side);
		//m_EdgeOrientation = p_EdgeOrientation;
		//
		//XMStoreFloat3(&m_forward, fwd);
		//XMVECTOR offsetToStartPos = XMVectorSet(0, m_ForceMoveY.back().x, m_ForceMoveZ.back().x,0);
		//offsetToStartPos = XMVector3Transform(-offsetToStartPos, a);
		//
		//XMVECTOR sp;
		//sp = vReachPointCenter + XMVectorSet(0,edgeY,0,0) + offsetToStartPos;
		//
		//XMVECTOR asp;
		//asp = XMLoadFloat3(&m_ForceMoveStartPos);
		//sp = sp - asp;
		//m_ForceMoveY[1].x += sp.m128_f32[1];
		//if(m_EdgeOrientation.x > 0.0f)
		//	m_ForceMoveZ[1].x += sp.m128_f32[0];
		//else
		//	m_ForceMoveZ[1].x += sp.m128_f32[2];
		// The goldener path code END

		// The golden path code
		vReachPointCenter = XMLoadFloat3(&playerOrigPos) - XMLoadFloat3(&p_BoxPos);
		vReachPointCenter = (XMVector3Dot(vReachPointCenter, vEdgeOrientation) * vEdgeOrientation) + XMLoadFloat3(&p_BoxPos);
		XMStoreFloat3(&m_CenterReachPos, vReachPointCenter);
		XMStoreFloat3(&m_Side, side);
		m_EdgeOrientation = p_EdgeOrientation;
		
		XMStoreFloat3(&m_forward, fwd);
		
		XMVECTOR offsetToStartPos = XMVectorSet(0, m_ForceMoveY.back().x, m_ForceMoveZ.back().x,0);
		offsetToStartPos = XMVector3Transform(-offsetToStartPos, a);
		
		XMVECTOR sp;
		sp = vReachPointCenter + offsetToStartPos;
		XMStoreFloat3(&m_ForceMoveStartPos, sp);
		setPosition(m_ForceMoveStartPos);
		// The golden path code END
		
		if (m_Network)
		{
			IConnectionController* con = m_Network->getConnectionToServer();
			if (con && con->isConnected())
			{
				tinyxml2::XMLPrinter printer;
				printer.OpenElement("Action");
				printer.OpenElement("Climb");
				printer.PushAttribute("Animation", p_ClimbId.c_str());

				pushVector(printer, "Center", m_CenterReachPos);
				pushVector(printer, "Orientation", m_EdgeOrientation);
				printer.CloseElement();
				printer.CloseElement();

				con->sendObjectAction(m_Actor.lock()->getId(), printer.CStr());
			}
		}

		aa.lock()->updateIKData(m_EdgeOrientation, m_CenterReachPos);
	}
}

void Player::setCurrentMana(float p_Mana)
{
	if(p_Mana > m_MaxMana)
		p_Mana = m_MaxMana;
	else if(p_Mana < 0.f)
		p_Mana = 0.f;

	m_PreviousMana = m_CurrentMana;
	m_CurrentMana = p_Mana;
	m_IsPreviousManaSet = true;
	
}

float Player::getPreviousMana()
{
	return m_PreviousMana;
}

float Player::getCurrentMana()
{
	return m_CurrentMana;
}

float Player::getMaxMana()
{
	return m_MaxMana;
}

void Player::setPosition(const XMFLOAT3 &p_Position)
{
	if(m_AllowedToMove)
	{
		Vector3 kneePos = p_Position;
		kneePos.y += getKneeHeight();
		kneePos.y -= m_ClimbOffset;
		m_Physics->setBodyPosition(getBody(), kneePos);
	}
}

XMFLOAT3 Player::getPosition(void) const
{
	Actor::ptr actor = m_Actor.lock();
	if (actor)
	{
		return actor->getPosition();
	}

	Vector3 pos = getCollisionCenter();
	pos.y -= getKneeHeight();
	return pos;
}

XMFLOAT3 Player::getEyePosition() const
{
	Actor::ptr actor = m_Actor.lock();
	if (actor)
	{
		std::shared_ptr<LookInterface> comp = actor->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
		if (comp)
		{
			return comp->getLookPosition();
		}
	}

	XMFLOAT3 eyePosition = getPosition();
	eyePosition.y += m_EyeHeight;
	return eyePosition;
}

XMFLOAT3 Player::getFootPosition(std::string p_Joint) const
{
	Actor::ptr actor = m_Actor.lock();
	if (actor)
	{
		std::shared_ptr<AnimationInterface> comp = actor->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId).lock();
		if (comp)
		{
			return comp->getJointPos(p_Joint);
		}
	}

	return getPosition();
}

XMFLOAT3 Player::getRightHandPosition() const
{
	Actor::ptr actor = m_Actor.lock();
	if (actor)
	{
		std::shared_ptr<AnimationInterface> comp = actor->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId).lock();
		if (comp)
		{
			return comp->getJointPos("R_UpperArm");
		}
	}

	XMFLOAT3 position = getPosition();
	position.y += m_EyeHeight;
	return position;
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
	return m_Height;
}

float Player::getChestHeight() const
{
	return m_Height * 0.75f;
}

float Player::getWaistHeight() const
{
	return m_Height * 0.5f;
}

float Player::getKneeHeight() const
{
	return m_Height * 0.25f;
}

void Player::setHeight(float p_Height)
{
	m_Height = p_Height;
}

BodyHandle Player::getBody() const
{
	return m_Actor.lock()->getBodyHandles()[0];
}

bool Player::getForceMove(void)
{
	return m_ForceMove;
}

Vector3 Player::getVelocity() const
{
	return m_Physics->getBodyVelocity(getBody());
}

Vector3 Player::getDirection() const
{
	Actor::ptr actor = m_Actor.lock();
	if (!actor)
	{
		return Vector3(0.f, 0.f, 0.f);
	}

	std::shared_ptr<MovementControlInterface> comp = actor->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
	if (!comp)
	{
		return Vector3(0.f, 0.f, 0.f);
	}

	return comp->getLocalDirection();
}

void Player::setJump(void)
{
	if(m_AllowedToMove)
	{
		if(m_Physics->getBodyInAir(getBody()))
		{
			m_JumpCount++;
		}
		
		Actor::ptr actor = m_Actor.lock();
		if (!actor)
			return;

		std::shared_ptr<MovementControlInterface> comp = actor->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
		std::shared_ptr<RunControlComponent> runComp = std::dynamic_pointer_cast<RunControlComponent>(comp);

		if(runComp && !runComp->getIsJumping() && m_JumpCount < m_JumpCountMax)
		{
			runComp->setIsJumping(true);

			Vector3 temp = m_Physics->getBodyVelocity(getBody());
			temp.y = 0.f;

			m_Physics->setBodyVelocity(getBody(), temp);

			m_Physics->applyForce(getBody(), Vector3(0.f, m_JumpForce, 0.f));
		}
	}
}

void Player::setDirection(Vector3 p_Direction)
{
	Actor::ptr actor = m_Actor.lock();
	if (!actor)
	{
		return;
	}

	std::shared_ptr<MovementControlInterface> comp = actor->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
	if (!comp)
	{
		return;
	}

	return comp->setLocalDirection(p_Direction);
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
	Actor::ptr actor = m_Actor.lock();
	if (!actor)
	{
		return XMFLOAT3(0.f, 1.f, 0.f);
	}
	
	std::shared_ptr<MovementControlInterface> comp = actor->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
	std::shared_ptr<RunControlComponent> runComp = std::dynamic_pointer_cast<RunControlComponent>(comp);
	if (!runComp)
	{
		return XMFLOAT3(0.f, 1.f, 0.f);
	}

	return runComp->getGroundNormal();
}

void Player::setGroundNormal(DirectX::XMFLOAT3 p_Normal)
{
	Actor::ptr actor = m_Actor.lock();
	if (!actor)
	{
		return;
	}
	
	std::shared_ptr<MovementControlInterface> comp = actor->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
	std::shared_ptr<RunControlComponent> runComp = std::dynamic_pointer_cast<RunControlComponent>(comp);
	if (!runComp)
	{
		return;
	}

	runComp->setGroundNormal(p_Normal);
}

void Player::setSpawnPosition(Vector3 p_Position)
{
	m_LastSafePosition = p_Position;
}

void Player::setClimbing(bool p_State)
{
	m_Climb = p_State;
}

void Player::setAllowedToMove(bool p_State)
{
	m_AllowedToMove = p_State;
}

const bool Player::getAllowedToMove() const
{
	return m_AllowedToMove;
}

void Player::jump(float dt)
{
	Actor::ptr actor = m_Actor.lock();
	if (!actor)
		return;

	std::shared_ptr<MovementControlInterface> comp = actor->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
	std::shared_ptr<RunControlComponent> runComp = std::dynamic_pointer_cast<RunControlComponent>(comp);

	if (!runComp)
		return;

	if(runComp->getIsJumping())
	{
		m_JumpTime += dt;
		if(m_JumpTime > m_JumpTimeMax)
		{
 			m_Physics->applyForce(getBody(), Vector3(0.f, -m_JumpForce, 0.f));
			runComp->setIsJumping(false);
			m_JumpTime = 0.f;
		}
	}
	if(!runComp->getIsJumping() && !m_Physics->getBodyInAir(getBody()))
	{
		m_JumpCount = 0;
	}
}
