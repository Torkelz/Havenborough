#pragma once
#include "SpellDefinition.h"
#include "IPhysics.h"

#include <DirectXMath.h>
#include <vector>

class SpellInstance
{
public:
	typedef std::shared_ptr<SpellInstance> ptr;

private:
	SpellDefinition::ptr m_SpellDefinition;

	float m_TimeLived; //in sec
	bool m_Collision;
	bool m_IsDead;
	Vector3 m_Velocity;

public:
	SpellInstance();
	~SpellInstance();

	void init(SpellDefinition::ptr p_SpellDefinition, Vector3 p_Direction);
	void update(float p_DeltaTime);
	
	void collisionHappened();
	float getRadius() const;
	Vector3 getVelocity() const;
	bool hasCollided() const;
	bool isDead() const;
	void spellHit(float p_DeltaTime, IPhysics* p_Physics, const HitData& p_Hit);

private:
	void explodeSpell(float p_DeltaTime, IPhysics* p_Physics, const HitData& p_Hit);

};

