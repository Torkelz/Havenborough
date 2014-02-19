#pragma once
#include <string>
#include <vector>
#include "..\3rd party\tinyxml2\tinyxml2.h"

class ResourceTranslator
{
private:
	
	std::vector<std::pair<std::string, std::vector<const std::pair<std::string, std::string>>>> m_MappedResources;
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

	void loadResourceList(std::istream& p_FileData);
private:
	std::pair<std::string, std::string> readValues(const tinyxml2::XMLElement* p_Element);
};