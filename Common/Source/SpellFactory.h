#pragma once
#include "SpellInstance.h"

#include <map>

class SpellFactory
{
private:
	IPhysics *m_Physics;

	std::map<std::string, SpellDefinition::ptr>  m_SpellDefinitionMap;

public:
	SpellFactory(IPhysics *p_Physics);
	~SpellFactory();

	void init();
	virtual SpellDefinition::ptr createSpellDefinition(const char* p_Spellname, const char* p_Filename);
	bool releaseSpellDefinition(const char* p_Spellname);
	virtual SpellInstance::ptr createSpellInstance(const std::string& p_Spell, Vector3 p_Direction);


	void readDefinitionFromFile(const char* p_Filename);


};

