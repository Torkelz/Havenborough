#pragma once
#include "SpellInstance.h"

class SpellFactory
{
public:
	SpellFactory();
	~SpellFactory();

	void init();
	virtual SpellDefinition::ptr createSpellDefinition();
	virtual SpellInstance::ptr createSpellInstance();

	void readDefinitionFromFile(const char* p_Filename);


};

