#include "SpellFactory.h"


SpellFactory::SpellFactory()
{
}


SpellFactory::~SpellFactory()
{
}

void SpellFactory::init()
{

}
SpellDefinition::ptr SpellFactory::createSpellDefinition(const char* p_Filename, const char* p_Spellname)
{
	//readDefinitionFromFile(p_Filename);

	SpellDefinition::ptr spell;
	spell.reset(new SpellDefinition());

	spell->effectRange = 100.f;
	spell->effectTime = 5.f;
	spell->maxTimeToLive = 60.f;
	spell->pushPower = 10.f;
	spell->spellName = "forcePush";
	spell->spellSize = 10.f;

	return spell;
}
SpellInstance::ptr SpellFactory::createSpellInstance(SpellDefinition::ptr p_Spell)
{
	SpellInstance::ptr instance(new SpellInstance);
	instance->init(p_Spell);

	return instance;
}

void SpellFactory::readDefinitionFromFile(const char* p_Filename)
{

}
