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
	IPhysics *m_Physics;
	
	BodyHandle m_Sphere;
	Vector3 m_SpellPosition; //in cm
	float m_TimeLived; //in sec
	bool m_Collision;

public:
	SpellInstance(IPhysics *p_Physics);
	~SpellInstance();

	void init(SpellDefinition::ptr p_SpellDefinition, Vector3 p_Direction, Vector3 p_SpellPosition);
	void update(float p_DeltaTime);
	
	void collisionHappened();

	void setPosition(Vector3 p_NewPosition);

private:
	void spellHit(SpellDefinition::ptr p_SpellDefinition, float p_DeltaTime);
	void explodeSpell(SpellDefinition::ptr p_SpellDefinition, float p_DeltaTime);
	void changeSphereRadius(float p_NewRadius);

};

