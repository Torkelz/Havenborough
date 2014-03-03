#include "SpellFactory.h"

#include "CommonExceptions.h"

SpellFactory::SpellFactory()
{
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
		//throw CommonException("Spell " + std::string(p_Spellname) + " already loaded", __LINE__, __FILE__);
	}

	//readDefinitionFromFile(p_Filename);

	SpellDefinition::ptr spell;
	spell.reset(new SpellDefinition());

	spell->m_Type = SpellDefinition::Type::EXPLOSION;
	spell->explosionRadius = 600.f;
	spell->effectTime = 0.1f;
	spell->maxTimeToLive = 5.f;
	spell->force = 5000.f;
	spell->minForce = 8000.f;
	spell->spellName = p_Spellname;
	spell->flyingSpellSize = 40.f;
	spell->flyForce = 5000.f;
	spell->manaCost = 55.f;

	m_SpellDefinitionMap[p_Spellname] = spell;

	return spell;
}

bool SpellFactory::releaseSpellDefinition(const char *p_SpellId)
{
	return m_SpellDefinitionMap.erase(p_SpellId) != 0;
}

SpellInstance::ptr SpellFactory::createSpellInstance(const std::string& p_Spell, Vector3 p_Direction)
{
	auto spellDef = m_SpellDefinitionMap.find(p_Spell);
	if (spellDef == m_SpellDefinitionMap.end())
	{
		throw CommonException("Can not create spell from a missing spell definition (" + p_Spell + ")", __LINE__, __FILE__);
	}

	SpellInstance::ptr instance(new SpellInstance);
	instance->init(spellDef->second, p_Direction);

	return instance;
}

void SpellFactory::readDefinitionFromFile(const char* p_Filename)
{

}

const float SpellFactory::getManaCostFromSpellDefinition(const char* p_SpellName)
{
	for(auto& sd : m_SpellDefinitionMap)
	{
		if(std::strcmp(sd.first.c_str(), p_SpellName) == 0)
		{
			return sd.second->manaCost;
		}
	}

	return FLT_MAX;
}