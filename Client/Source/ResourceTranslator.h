#pragma once

#include <string>
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN

class ResourceTranslator
{
private:
	
public:
	ResourceTranslator();
	~ResourceTranslator();

	std::string translate(std::string p_ResourceType, std::string p_ResourceName);
};

