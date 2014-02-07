#include "SpellFactory.h"


SpellFactory::SpellFactory(IPhysics *p_Physics)
{
	m_Physics = p_Physics;
}


SpellFactory::~SpellFactory()
{
}

void SpellFactory::init()
{

}

SpellDefinition::ptr SpellFactory::createSpellDefinition(const char* p_Spellname, const char* p_Filename)
{
	//readDefinitionFromFile(p_Filename);

	SpellDefinition::ptr spell;
	spell.reset(new SpellDefinition());

	spell->m_type = SpellDefinition::Type::EXPLOSION;
	spell->explosionRadius = 1.f;
	spell->effectTime = 0.f;
	spell->maxTimeToLive = 60.f;
	spell->force = 1.f;
	spell->minForce = 0.1f;
	spell->spellName = "forcePush";
	spell->flyingSpellSize = 0.1f;
	spell->flyForce = 1.f;

	return spell;
}

SpellInstance::ptr SpellFactory::createSpellInstance(SpellDefinition::ptr p_Spell, Vector3 p_Direction, Vector3 p_SpellPosition)
{
	SpellInstance::ptr instance(new SpellInstance(m_Physics));
	instance->init(p_Spell, p_Direction, p_SpellPosition);

	return instance;
}

void SpellFactory::readDefinitionFromFile(const char* p_Filename)
{

}
