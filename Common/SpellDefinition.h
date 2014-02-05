#pragma once

#include <memory>
#include <string>

struct SpellDefinition
{
public:
	typedef std::shared_ptr<SpellDefinition> ptr;

	//Sphere *spellShpere;

	std::string spellName;
	float effectRange;
	float spellSize;
	float pushPower;
	//float slowPower;
	//float manaCost;

	SpellDefinition()
		:	//spellShpere(nullptr),
			spellName("NO NAME FOUND"),
			effectRange(0.f),
			spellSize(1.f),
			pushPower(0.f)
			//slowPower(0.f),
			//manaCost(0.f)
	{}

	~SpellDefinition(){}
};
