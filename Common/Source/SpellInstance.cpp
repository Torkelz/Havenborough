#include "SpellInstance.h"


SpellInstance::SpellInstance()
{
	m_SpellPosition = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_SpellVelocity = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_SpellColor =  DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_TimeLived = 0.f;
	m_Collision = false;
}


SpellInstance::~SpellInstance()
{
}

void SpellInstance::init(SpellDefinition::ptr p_SpellDefinition)
{
	m_SpellDefinition = p_SpellDefinition;

	m_Sphere = m_Physics->createSphere(1.f, false, Vector3(m_SpellPosition.x, m_SpellPosition.y, m_SpellPosition.z), m_SpellDefinition->flyingSpellSize);
}

void SpellInstance::update(float p_DeltaTime)
{

	if(m_Collision || m_TimeLived >= m_SpellDefinition->maxTimeToLive)
	{
		spellHit(m_SpellDefinition);
	}
	else
	{
		m_TimeLived += p_DeltaTime;
	}
}

void SpellInstance::spellHit(SpellDefinition::ptr p_SpellDefinition)
{
	changeSphereRadius(m_SpellDefinition->explosionRadius);
	m_SpellVelocity = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	explodeSpell(p_SpellDefinition);
}

void SpellInstance::explodeSpell(SpellDefinition::ptr p_SpellDefinition)
{


	for(unsigned i = 0; i < m_Physics->getHitDataSize(); i++)
	{
		if(m_Physics->getHitDataAt(i).collider == m_Sphere)
		{
			HitData temp = m_Physics->getHitDataAt(i);

			float forceFactor = temp.colLength / m_SpellDefinition->explosionRadius;

			Vector4 vTemp = temp.colNorm * (m_SpellDefinition->minForce + forceFactor * m_SpellDefinition->force);

			m_Physics->applyForce(temp.collisionVictim, vTemp.xyz() );
		}
	}
}

void SpellInstance::changeSphereRadius(float p_NewRadius)
{
	m_Physics->setBodyScale(m_Sphere, Vector3(p_NewRadius, 0.f, 0.f));
}

void SpellInstance::setPosition(DirectX::XMFLOAT4 p_NewPosition)
{
	m_SpellPosition = p_NewPosition;
}

void SpellInstance::setVelocity(DirectX::XMFLOAT4 p_NewVelocity)
{
	m_SpellVelocity = p_NewVelocity;
}

void SpellInstance::setColor(DirectX::XMFLOAT4 p_NewColor)
{
	m_SpellColor = p_NewColor;
}