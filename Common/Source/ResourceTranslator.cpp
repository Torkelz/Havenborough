#include "ResourceTranslator.h"
#include "CommonExceptions.h"

ResourceTranslator::ResourceTranslator(){}

ResourceTranslator::~ResourceTranslator(){}

void ResourceTranslator::loadResourceList(std::istream& p_FileData)
{
	std::vector<char> buffer;
	p_FileData >> std::noskipws;
	std::copy(std::istream_iterator<char>(p_FileData), std::istream_iterator<char>(), std::back_inserter(buffer));

	tinyxml2::XMLDocument resourceList;

	tinyxml2::XMLError error = resourceList.Parse(buffer.data(), buffer.size());
	if(error)
	{
		throw ResourceManagerException("Error parsing resource data. Error code: " + std::to_string(error), __LINE__, __FILE__);
	}
	tinyxml2::XMLElement* resourceFile = resourceList.FirstChildElement("Resources");
	if(resourceFile == nullptr)
	{
		throw ResourceManagerException("File was not of Resource .xml type.", __LINE__, __FILE__);
	}
	
	for(const tinyxml2::XMLElement* resourceType = resourceFile->FirstChildElement("ResourceType"); resourceType; resourceType = resourceType->NextSiblingElement("ResourceType"))
	{
		const char* type = resourceType->Attribute("Type");
		if (!type)
			continue;

		m_MappedResources.push_back(std::make_pair(type, std::vector<const std::pair<std::string, std::string>>()));
		auto& map = m_MappedResources.back().second;

		for(const tinyxml2::XMLElement* resource = resourceType->FirstChildElement("Resource"); resource; resource = resource->NextSiblingElement("Resource"))
		{
			map.push_back(readValues(resource));
		}
	}
}

std::string ResourceTranslator::translate(std::string p_ResourceType, std::string p_ResourceName)
{
	for (const auto& map : m_MappedResources)
	{
		if (p_ResourceType == map.first)
		{
			for (const auto& m : map.second)
			{
				if (m.first == p_ResourceName)
				{
					return m.second;
				}
			}
		}
	}

	throw ResourceManagerException("Unknown resource: '" + p_ResourceType + ":" + p_ResourceName + "'", __LINE__, __FILE__);
}

std::pair<std::string, std::string> ResourceTranslator::readValues(const tinyxml2::XMLElement* p_Element)
{
	const char* name, *path;
	name = p_Element->Attribute("Name");
	if(name == nullptr)
	{
		ResourceManagerException("Resource attribute was not found!", __LINE__, __FILE__);
	}
	path = p_Element->Attribute("Path");
	if(path == nullptr)
	{
		ResourceManagerException("Resource path was not read!", __LINE__, __FILE__);
	}
	return std::make_pair<std::string, std::string>(std::string(name), std::string(path));
}