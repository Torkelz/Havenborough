#pragma once
#include "SpellInstance.h"

#include <map>

class SpellFactory
{
private:
	std::map<std::string, SpellDefinition::ptr>  m_SpellDefinitionMap;

public:
	
	/**
	 * Basic constructor and deconstructor
	 */
	SpellFactory();
	~SpellFactory();

	/**
	 * Initialization function to start up the spell factory.
	 * (Currently empty)
	 */
	void init();
	
	/**
	 * Called to make add a definition to the list. Read from file.
	 * 
	 * @param p_Spellname are what the new spell definition are to be called
	 * @param p_Filename are the name of the file to read from
	 * @return a pointer to the new definition just added to the list
	 */
	virtual SpellDefinition::ptr createSpellDefinition(const char* p_Spellname, const char* p_Filename);
	
	/**
	 * Called to release a specified definition from the list.
	 * 
	 * @param p_Spellname used as an identifier to know what definition to remove from the list
	 * @return true if it did not fail to remove the definition
	 */
	bool releaseSpellDefinition(const char* p_Spellname);
	
	/**
	 * Called to make an instance from a definition.
	 * 
	 * @param p_Spell used as an identifier to know what kind of definition to use
	 * @param p_Direction what direction the fly force should be added to
	 * @return a pointer to the new instance just created
	 */
	virtual SpellInstance::ptr createSpellInstance(const std::string& p_Spell, Vector3 p_Direction);
	
	/**
	 * Function used to read the file containing the spell definitions
	 * 
	 * @param p_Filename are the name of the file to read from
	 */
	void readDefinitionFromFile(const char* p_Filename);
};

