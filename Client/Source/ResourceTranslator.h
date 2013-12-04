#pragma once

#include <string>

class ResourceTranslator
{
private:
	
public:
	ResourceTranslator();
	~ResourceTranslator();

	std::string translate(std::string p_ResourceType, std::string p_ResourceName);
};

