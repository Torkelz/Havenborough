#include "SpellInstance.h"

SpellInstance::SpellInstance()
{
	m_TimeLived = 0.f;
	m_Collision = false;
	m_IsColliding = false;
	m_IsDead = false;
}

SpellInstance::~SpellInstance()
{
}

void SpellInstance::init(SpellDefinition::ptr p_SpellDefinition, Vector3 p_Direction)
{
	m_SpellDefinition = p_SpellDefinition;
	
	m_Velocity = p_Direction * m_SpellDefinition->flyForce;
}

void SpellInstance::update(float p_DeltaTime)
{
	m_TimeLived += p_DeltaTime;

	m_IsColliding = false;

	if(m_Collision)
	{
		if (m_TimeLived >= m_SpellDefinition->effectTime)
		{
			m_IsDead = true;
			return;
		}
		return;
	}

	if(m_TimeLived >= m_SpellDefinition->maxTimeToLive)
	{
		collisionHappened();
	}
}

void SpellInstance::collisionHappened()
{
	m_IsColliding = true;
	m_Collision = true;

	m_TimeLived = 0.f;
}

Vector3 SpellInstance::getSize() const
{
	return m_SpellDefinition->flyingSpellSize;
}

float SpellInstance::getRadius() const
{
	return m_SpellDefinition->explosionRadius;
}

Vector3 SpellInstance::getVelocity() const
{
	return m_Velocity;
}

bool SpellInstance::hasCollided() const
{
	return m_Collision;
}

bool SpellInstance::isDead() const
{
	return m_IsDead;
}

void SpellInstance::spellHit(IPhysics* p_Physics, const HitData& p_Hit, BodyHandle p_CasterBody)
{
	switch (m_SpellDefinition->m_Type)
	{
	case SpellDefinition::Type::EXPLOSION:
		{
			explodeSpell(p_Physics, p_Hit, p_CasterBody);
			break;
		}

	default:
		{
			break;
		}
	}


}

bool SpellInstance::isColliding() const
{
	return m_IsColliding;
}

void SpellInstance::explodeSpell(IPhysics* p_Physics, const HitData& p_Hit, BodyHandle p_CasterBody)
{
	float modifier = -1.f;
	float casterEffectModifier = 1.4f;

	if(p_Hit.IDInBody == 1)
	{
		float forceFactor = p_Hit.colLength / m_SpellDefinition->explosionRadius;
		Vector4 vTemp = (p_Hit.colNorm * (m_SpellDefinition->minForce + forceFactor * m_SpellDefinition->force) * modifier);
		
		if (p_Hit.collider == p_CasterBody)
		{
			p_Physics->applyImpulse(p_Hit.collider, vTemp.xyz()  * p_Physics->getTimestep());
		} 
		else
		{
			p_Physics->applyImpulse(p_Hit.collider, vTemp.xyz() * casterEffectModifier  * p_Physics->getTimestep());
		}


	}
}
