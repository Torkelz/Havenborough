#pragma once
#include "SpellInstance.h"

class SpellFactory
{
private:
	IPhysics *m_Physics;

public:
	SpellFactory(IPhysics *p_Physics);
	~SpellFactory();

	void init();
	virtual SpellDefinition::ptr createSpellDefinition(const char* p_Spellname, const char* p_Filename);
	virtual SpellInstance::ptr createSpellInstance(SpellDefinition::ptr p_Spell, Vector3 p_Direction, Vector3 p_SpellPosition);

	void readDefinitionFromFile(const char* p_Filename);


};

