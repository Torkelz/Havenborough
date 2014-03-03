#include "RunControlComponent.h"

RunControlComponent::RunControlComponent()
	: m_Physics(nullptr),
	m_PhysicsComp(),
	m_IsJumping(false),
	m_MaxSpeed(0.f),
	m_AccConstant(0.f),
	m_GroundNormal(0.f, 1.f, 0.f)
{
}

void RunControlComponent::initialize(const tinyxml2::XMLElement* p_Data)
{
	p_Data->QueryAttribute("MaxSpeed", &m_MaxSpeed);
	p_Data->QueryAttribute("Acceleration", &m_AccConstant);
}

void RunControlComponent::serialize(tinyxml2::XMLPrinter& p_Printer) const
{
	p_Printer.OpenElement("RunControl");
	p_Printer.PushAttribute("MaxSpeed", m_MaxSpeed);
	p_Printer.PushAttribute("Acceleration", m_AccConstant);
	p_Printer.CloseElement();
}

void RunControlComponent::postInit()
{
	m_PhysicsComp = m_Owner->getComponent<PhysicsInterface>(PhysicsInterface::m_ComponentId);
}

void RunControlComponent::move(float p_DeltaTime)
{
	std::shared_ptr<PhysicsInterface> comp = m_PhysicsComp.lock();

	if (!comp)
	{
		return;
	}

	BodyHandle body = comp->getBodyHandle();

	if (!m_Physics->getBodyInAir(body) || getIsJumping())
	{
		using namespace DirectX;
		XMFLOAT3 velocity = m_Physics->getBodyVelocity(body);
		XMVECTOR currentVelocity = XMLoadFloat3(&velocity);	// cm/s

		XMFLOAT3 maxVelocity(m_RunningDirection.x * getMaxSpeed(),
			0.f, m_RunningDirection.z * getMaxSpeed());	// cm/s
	
		if (getIsJumping())
		{
			maxVelocity.y = getMaxSpeed() * 0.5f;
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

		m_Physics->applyImpulse(body, fForceDiff);

		m_RunningDirection = Vector3();
	}
}

Vector3 RunControlComponent::getLocalDirection() const
{
	return m_RunningDirection;
}

void RunControlComponent::setLocalDirection(const Vector3& p_Direction)
{
	m_RunningDirection = p_Direction;
}

float RunControlComponent::getMaxSpeed() const
{
	return m_MaxSpeed;
}

void RunControlComponent::setMaxSpeed(float p_Speed)
{
	m_MaxSpeed = p_Speed;
}

bool RunControlComponent::getIsJumping() const
{
	return m_IsJumping;
}

void RunControlComponent::setIsJumping(bool p_Jumping)
{
	m_IsJumping = p_Jumping;
}

Vector3 RunControlComponent::getGroundNormal() const
{
	return m_GroundNormal;
}

void RunControlComponent::setGroundNormal(const Vector3& p_Normal)
{
	m_GroundNormal = p_Normal;
}

void RunControlComponent::setPhysics(IPhysics* p_Physics)
{
	m_Physics = p_Physics;
}
