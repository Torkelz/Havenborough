#pragma once

#include <memory>
#include <string>

struct SpellDefinition
{
public:
	/**
	 * A flag to tell what code path to take for the spell.
	 */
	enum class Type
	{
		EXPLOSION,
	};

	/**
	 * 
	 */
	typedef std::shared_ptr<SpellDefinition> ptr;

	/**
	 * Used as an identifier for the definition itself.
	 */
	std::string spellName;

	/**
	 * A flag to tell what code path to take for the spell.
	 */
	Type m_Type;
	
	/**
	 * How long the spell can fly in the air before 
	 * it will move on to the next stage.
	 * in sec
	 */
	float maxTimeToLive;
	
	/**
	 * How big the the effect radius the spell will have in the next stage.
	 * in m
	 */
	float explosionRadius;
	
	/**
	 * How big the spell are when it's flying in the air.
	 * in m
	 */
	float flyingSpellSize;
	
	/**
	 * How fast a spell will fly in the air.
	 * in N
	 */
	float flyForce;
	
	/**
	 * How long time the next stage of the spell will last.
	 * in sec
	 */
	float effectTime;
	
	/**
	 * How much fore power that will be scaled by the force factor.
	 * in N
	 */
	float force;
	
	/**
	 * How much "pure" force to be added on top of the deviated force.
	 * in N
	 */
	float minForce;
	
	/**
	 * How much it will effect the speed of objects.
	 */
	//float slowFactor;
	
	/**
	 * how much the spell will cost on use.
	 */
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
			//slowFactor(-1.f),
			//manaCost(-1.f)
	{}

	~SpellDefinition(){}
};
