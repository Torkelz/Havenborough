#include "SpellInstance.h"


SpellInstance::SpellInstance(IPhysics *p_Physics)
{
	m_Physics = p_Physics;
	m_SpellPosition = Vector3(0.f, 0.f, 0.f);
	m_TimeLived = 0.f;
	m_Collision = false;
}


SpellInstance::~SpellInstance()
{
	if (m_Sphere != 0)
	{
		m_Physics->releaseBody(m_Sphere);
	}
}

void SpellInstance::init(SpellDefinition::ptr p_SpellDefinition, Vector3 p_Direction, Vector3 p_SpellPosition)
{
	m_SpellDefinition = p_SpellDefinition;
	m_SpellPosition = p_SpellPosition;
	
	Vector3 ForceDirection = p_Direction * m_SpellDefinition->flyForce;

	m_Sphere = m_Physics->createSphere(0.f, false, m_SpellPosition, m_SpellDefinition->flyingSpellSize);
	m_Physics->setBodyVelocity(m_Sphere, ForceDirection);
}

void SpellInstance::update(float p_DeltaTime)
{
	if (m_Sphere == 0)
	{
		return;
	}

	m_TimeLived += p_DeltaTime;

	if(m_Collision)
	{
		spellHit(m_SpellDefinition, p_DeltaTime);
		return;
	}

	if(m_TimeLived >= m_SpellDefinition->maxTimeToLive)
	{
		m_Collision = true;
	}
	else
	{
		for(unsigned i = 0; i < m_Physics->getHitDataSize(); i++)
		{
			if(m_Physics->getHitDataAt(i).collider == m_Sphere)
			{
				m_Collision = true;
				break;
			}
		}
	}

	if(m_Collision)
	{
		Vector3 currentPosition = m_Physics->getBodyPosition(m_Sphere);
		m_Physics->releaseBody(m_Sphere);
		m_Sphere = m_Physics->createSphere(0.f, true, currentPosition, m_SpellDefinition->explosionRadius);
		m_Physics->setBodyCollisionResponse(m_Sphere, false);
		
		m_TimeLived = 0.f;
	}
}

void SpellInstance::spellHit(SpellDefinition::ptr p_SpellDefinition, float p_DeltaTime)
{
	if (m_TimeLived >= m_SpellDefinition->effectTime)
	{
		m_Physics->releaseBody(m_Sphere);
		m_Sphere = 0;
			return;
	}

	switch (m_SpellDefinition->m_type)
	{
	case SpellDefinition::Type::EXPLOSION:
		{
			explodeSpell(p_SpellDefinition, p_DeltaTime);
			break;
		}

	default:
		{
			break;
		}
	}
}

void SpellInstance::explodeSpell(SpellDefinition::ptr p_SpellDefinition, float p_DeltaTime)
{	
	for(unsigned i = 0; i < m_Physics->getHitDataSize(); i++)
	{
		if(m_Physics->getHitDataAt(i).collisionVictim == m_Sphere)
		{
			HitData temp = m_Physics->getHitDataAt(i);

			float forceFactor = temp.colLength / m_SpellDefinition->explosionRadius;

			Vector4 vTemp = temp.colNorm * (m_SpellDefinition->minForce + forceFactor * m_SpellDefinition->force);

			m_Physics->applyImpulse(temp.collider, vTemp.xyz() * p_DeltaTime);
		}
	}
}

void SpellInstance::setPosition(Vector3 p_NewPosition)
{
	m_SpellPosition = p_NewPosition;
}

void SpellInstance::collisionHappened()
{
	m_Collision = true;
}

void SpellInstance::changeSphereRadius(float p_NewRadius)
{
	m_Physics->setBodyScale(m_Sphere, Vector3(p_NewRadius / m_SpellDefinition->flyingSpellSize, 0.f, 0.f));
}

