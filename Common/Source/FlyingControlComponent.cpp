#include "FlyingControlComponent.h"

FlyingControlComponent::FlyingControlComponent()
	: m_Physics(nullptr),
	m_PhysicsComp(),
	m_MaxSpeed(0.f),
	m_AccConstant(0.f)
{
}

void FlyingControlComponent::initialize(const tinyxml2::XMLElement* p_Data)
{
	p_Data->QueryAttribute("MaxSpeed", &m_MaxSpeed);
	p_Data->QueryAttribute("Acceleration", &m_AccConstant);
}

void FlyingControlComponent::serialize(tinyxml2::XMLPrinter& p_Printer) const
{
	p_Printer.OpenElement("FlyingControl");
	p_Printer.PushAttribute("MaxSpeed", m_MaxSpeed);
	p_Printer.PushAttribute("Acceleration", m_AccConstant);
	p_Printer.CloseElement();
}

void FlyingControlComponent::postInit()
{
	m_PhysicsComp = m_Owner->getComponent<PhysicsInterface>(PhysicsInterface::m_ComponentId);
	
	std::shared_ptr<PhysicsInterface> phys = m_PhysicsComp.lock();
	if (phys)
	{
		m_Physics->setBodyGravity(phys->getBodyHandle(), 0.f);
	}
}

void FlyingControlComponent::move(float p_DeltaTime)
{
	std::shared_ptr<PhysicsInterface> comp = m_PhysicsComp.lock();

	if (!comp)
		return;
	
	BodyHandle body = comp->getBodyHandle();

	using namespace DirectX;
	XMFLOAT3 velocity = m_Physics->getBodyVelocity(body);
	XMVECTOR currentVelocity = XMLoadFloat3(&velocity);	// cm/s

	XMFLOAT3 maxVelocity(m_FlyingDirection * getMaxSpeed());	// cm/s

	XMVECTOR vMaxVelocity = XMLoadFloat3(&maxVelocity);

	XMVECTOR diffVel = vMaxVelocity - currentVelocity;	// cm/s
	
	XMVECTOR force = diffVel / 100.f * m_AccConstant;		// kg * m/s^2

	XMVECTOR forceDiffImpulse = force * p_DeltaTime;	// kg * m/s

	XMFLOAT3 fForceDiff;
	XMStoreFloat3(&fForceDiff, forceDiffImpulse);

	m_Physics->applyImpulse(body, fForceDiff);

	m_FlyingDirection = Vector3();
}

Vector3 FlyingControlComponent::getLocalDirection() const
{
	return m_FlyingDirection;
}

void FlyingControlComponent::setLocalDirection(const Vector3& p_Direction)
{
	m_FlyingDirection = p_Direction;
}

float FlyingControlComponent::getMaxSpeed() const
{
	return m_MaxSpeed;
}

void FlyingControlComponent::setMaxSpeed(float p_Speed)
{
	m_MaxSpeed = p_Speed;
}

void FlyingControlComponent::setPhysics(IPhysics* p_Physics)
{
	m_Physics = p_Physics;
}
