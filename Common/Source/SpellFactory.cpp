#include "SpellFactory.h"

#include "CommonExceptions.h"

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
	if (m_SpellDefinitionMap.count(p_Spellname) > 0)
	{
		throw CommonException("Spell " + std::string(p_Spellname) + " already loaded", __LINE__, __FILE__);
	}

	//readDefinitionFromFile(p_Filename);

	SpellDefinition::ptr spell;
	spell.reset(new SpellDefinition());

	spell->m_type = SpellDefinition::Type::EXPLOSION;
	spell->explosionRadius = 500.f;
	spell->effectTime = 0.5f;
	spell->maxTimeToLive = 60.f;
	spell->force = 5000.f;
	spell->minForce = 500.f;
	spell->spellName = p_Spellname;
	spell->flyingSpellSize = 10.f;
	spell->flyForce = 1000.f;

	m_SpellDefinitionMap[p_Spellname] = spell;

	return spell;
}

bool SpellFactory::releaseSpellDefinition(const char *p_SpellId)
{
	return m_SpellDefinitionMap.erase(p_SpellId) != 0;
}

SpellInstance::ptr SpellFactory::createSpellInstance(const std::string& p_Spell, Vector3 p_Direction, Vector3 p_SpellPosition)
{
	auto spellDef = m_SpellDefinitionMap.find(p_Spell);
	if (spellDef == m_SpellDefinitionMap.end())
	{
		throw CommonException("Can not create spell from a missing spell definition (" + p_Spell + ")", __LINE__, __FILE__);
	}

	SpellInstance::ptr instance(new SpellInstance(m_Physics));
	instance->init(spellDef->second, p_Direction, p_SpellPosition);

	return instance;
}

void SpellFactory::readDefinitionFromFile(const char* p_Filename)
{

}
