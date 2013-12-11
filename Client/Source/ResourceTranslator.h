#pragma once

#include <string>


class ResourceTranslator
{
private:
	
public:
	ResourceTranslator();
	~ResourceTranslator();

	/**
	 * Translates a resource name to a filepath
	 * @param p_ResourceType type of resource
	 * @param p_ResourceName name of the resource
	 * @return a filepath to the resource.
	 */
	std::string translate(std::string p_ResourceType, std::string p_ResourceName);
};

