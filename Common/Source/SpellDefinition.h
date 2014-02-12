#pragma once

#include <memory>
#include <string>

struct SpellDefinition
{
public:
	enum class Type
	{
		EXPLOSION,

	};

	typedef std::shared_ptr<SpellDefinition> ptr;
	
	std::string spellName;
	Type m_type;
	float maxTimeToLive; //in sec
	float explosionRadius; //in m, explosion radius
	float flyingSpellSize; //in m, radius on the flying sphere
	float flyForce;
	float effectTime; //in sec
	float force;
	float minForce;
	//float slowPower;
	//float manaCost;

	SpellDefinition()
		:	spellName("NO NAME FOUND"),
			maxTimeToLive(-1.f),
			explosionRadius(-1.f),
			flyingSpellSize(-1.f),
			flyForce(-1.f),
			effectTime(-1.f),
			force(-1.f),
			minForce(-1.f)
			//slowPower(-1.f),
			//manaCost(-1.f)
	{}

	~SpellDefinition(){}
};
