#include "Body.h"

using namespace DirectX;

Body::BodyHandle Body::m_NextHandle = 1;

Body::BodyHandle Body::getNextHandle()
{
	return m_NextHandle++;
}

Body::Body(float p_mass, std::unique_ptr<BoundingVolume> p_BoundingVolume, bool p_IsImmovable)
	: m_Handle(getNextHandle()),
	  m_Volume(std::move(p_BoundingVolume))
{
	m_Mass = p_mass;	
	m_Position			= *m_Volume->getPosition();;
	m_NetForce			= XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_Velocity			= XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_Acceleration		= XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_LastAcceleration	= XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_AvgAcceleration	= XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_NewAcceleration	= XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_Gravity			= 0.f;

	m_InAir				= true;
	m_IsImmovable = p_IsImmovable;
}

Body::Body(Body &&p_Other)
	: m_Handle(p_Other.m_Handle),
	  m_Volume(std::move(p_Other.m_Volume)),
	  m_Mass(p_Other.m_Mass),
	  m_Position(p_Other.m_Position),
	  m_NetForce(p_Other.m_NetForce),
	  m_Velocity(p_Other.m_Velocity),
	  m_Acceleration(p_Other.m_Acceleration),
	  m_LastAcceleration(p_Other.m_LastAcceleration),
	  m_AvgAcceleration(p_Other.m_AvgAcceleration),
	  m_NewAcceleration(p_Other.m_NewAcceleration),
	  m_Gravity(p_Other.m_Gravity),
	  m_InAir(p_Other.m_InAir),
	  m_IsImmovable(p_Other.m_IsImmovable)
{}

Body::~Body()
{

}

void Body::addForce(XMFLOAT4 p_Force)
{
	XMVECTOR tempForce, tempNetForce;
	tempForce = XMLoadFloat4(&p_Force);
	tempNetForce = XMLoadFloat4(&m_NetForce);

	tempNetForce += tempForce;

	XMStoreFloat4(&m_NetForce, tempNetForce);
}

void Body::update(float p_DeltaTime)
{
	if(m_IsImmovable)
		return;

	m_Acceleration = calculateAcceleration();

	XMFLOAT4 relativePos = XMFLOAT4(0.f, 0.f, 0.f, 1.f);

	relativePos.x = m_Velocity.x * p_DeltaTime * 100 + (0.5f * m_LastAcceleration.x * (p_DeltaTime*p_DeltaTime));
	relativePos.y = m_Velocity.y * p_DeltaTime * 100 + (0.5f * m_LastAcceleration.y * (p_DeltaTime*p_DeltaTime));
	relativePos.z = m_Velocity.z * p_DeltaTime * 100 + (0.5f * m_LastAcceleration.z * (p_DeltaTime*p_DeltaTime));

	m_Position.x += relativePos.x;
	m_Position.y += relativePos.y;
	m_Position.z += relativePos.z;

	m_NewAcceleration = calculateAcceleration();
	XMVECTOR tempAvg, tempNew, tempLast;
	tempAvg = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	tempNew = XMLoadFloat4(&m_NewAcceleration);
	tempLast = XMLoadFloat4(&m_LastAcceleration);

	tempAvg += (tempLast + tempNew) * 0.5f;
	tempAvg *= p_DeltaTime;

	XMStoreFloat4(&m_AvgAcceleration, tempAvg);

	m_Velocity.x += m_AvgAcceleration.x;
	m_Velocity.y += m_AvgAcceleration.y;
	m_Velocity.z += m_AvgAcceleration.z;

	m_LastAcceleration = m_Acceleration;

	updateBoundingVolumePosition(relativePos);
	
	addGravity();
}

void Body::updateBoundingVolumePosition(XMFLOAT4 p_Position)
{
	XMMATRIX matTrans = XMMatrixTranslation(p_Position.x, p_Position.y, p_Position.z);
	XMFLOAT4X4 tempTrans;

	XMStoreFloat4x4(&tempTrans, matTrans);

	m_Volume->updatePosition(tempTrans);
}

XMFLOAT4 Body::calculateAcceleration()
{
	XMFLOAT4 acc;

	if(m_Mass == 0)
	{
		return XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	}
	
	acc.x = m_NetForce.x/m_Mass;
	acc.y = m_NetForce.y/m_Mass;
	acc.z = m_NetForce.z/m_Mass;

	return acc;
}

void Body::addGravity()
{
	m_NetForce.y = m_Mass * -m_Gravity;
}

void Body::setGravity(float p_Gravity)
{
	m_Gravity = p_Gravity;
}

bool Body::getInAir()
{
	return m_InAir;
}

void Body::setInAir(bool p_bool)
{
	m_InAir = p_bool;
}

bool Body::getIsImmovable()
{
	return m_IsImmovable;
}

BoundingVolume* Body::getVolume()
{
	return m_Volume.get();
}

XMFLOAT4 Body::getVelocity()
{
	return m_Velocity;
}

void Body::setVelocity(XMFLOAT4 p_Velocity)
{
	m_Velocity.x = p_Velocity.x;
	m_Velocity.y = p_Velocity.y;
	m_Velocity.z = p_Velocity.z;
}

XMFLOAT4 Body::getPosition()
{
	return m_Position;
}

void Body::setPosition(XMFLOAT4 p_Position)
{
	XMFLOAT4 diffPos(p_Position.x - m_Position.x, p_Position.y - m_Position.y, p_Position.z - m_Position.z, p_Position.w - m_Position.w);
	updateBoundingVolumePosition(diffPos);
	m_Position.x = p_Position.x;
	m_Position.y = p_Position.y;
	m_Position.z = p_Position.z;
	m_Position.w = p_Position.w;
}