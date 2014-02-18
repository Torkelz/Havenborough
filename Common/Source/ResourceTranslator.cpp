#include "ResourceTranslator.h"
#include "CommonExceptions.h"

#include <vector>

ResourceTranslator::ResourceTranslator(){}

ResourceTranslator::~ResourceTranslator(){}

void ResourceTranslator::loadResourceList(std::istream& p_FileData)
{
	std::vector<const std::pair<std::string, std::string>> modelMap;
	std::vector<const std::pair<std::string, std::string>> volumeMap;
	std::vector<const std::pair<std::string, std::string>> textureMap;
	std::vector<const std::pair<std::string, std::string>> soundMap;
	std::vector<const std::pair<std::string, std::string>> particleMap;
	std::vector<const std::pair<std::string, std::string>> spellMap;
	std::vector<const std::pair<std::string, std::string>> animationMap;
	std::vector<char> buffer;
	p_FileData >> std::noskipws;
	std::copy(std::istream_iterator<char>(p_FileData), std::istream_iterator<char>(), std::back_inserter(buffer));
	buffer.push_back('\0');

	tinyxml2::XMLDocument resourceList;

	tinyxml2::XMLError error = resourceList.Parse(buffer.data());
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
		if(resourceType->Attribute("Type", "model"))
		{
			for(const tinyxml2::XMLElement* resource = resourceType->FirstChildElement("Resource"); resource; resource = resource->NextSiblingElement("Resource"))
			{
				modelMap.push_back(readValues(resource));
			}
		}
		else if(resourceType->Attribute("Type", "volume"))
		{
			for(const tinyxml2::XMLElement* resource = resourceType->FirstChildElement("Resource"); resource; resource = resource->NextSiblingElement("Resource"))
			{
				volumeMap.push_back(readValues(resource));
			}
		}
		else if(resourceType->Attribute("Type", "texture"))
		{
			for(const tinyxml2::XMLElement* resource = resourceType->FirstChildElement("Resource"); resource; resource = resource->NextSiblingElement("Resource"))
			{
				textureMap.push_back(readValues(resource));
			}
		}
		else if(resourceType->Attribute("Type", "sound"))
		{
			for(const tinyxml2::XMLElement* resource = resourceType->FirstChildElement("Resource"); resource; resource = resource->NextSiblingElement("Resource"))
			{
				soundMap.push_back(readValues(resource));
			}
		}
		else if(resourceType->Attribute("Type", "particle"))
		{
			for(const tinyxml2::XMLElement* resource = resourceType->FirstChildElement("Resource"); resource; resource = resource->NextSiblingElement("Resource"))
			{
				particleMap.push_back(readValues(resource));
			}
		}
		else if(resourceType->Attribute("Type", "spell"))
		{
			for(const tinyxml2::XMLElement* resource = resourceType->FirstChildElement("Resource"); resource; resource = resource->NextSiblingElement("Resource"))
			{
				spellMap.push_back(readValues(resource));
			}
		}
		else if(resourceType->Attribute("Type", "animation"))
		{
			for(const tinyxml2::XMLElement* resource = resourceType->FirstChildElement("Resource"); resource; resource = resource->NextSiblingElement("Resource"))
			{
				animationMap.push_back(readValues(resource));
			}
		}
	}
	m_MappedResources.push_back(std::make_pair("model", modelMap));
	m_MappedResources.push_back(std::make_pair("texture", textureMap));
	m_MappedResources.push_back(std::make_pair("volume", volumeMap));
	m_MappedResources.push_back(std::make_pair("sound", soundMap));
	m_MappedResources.push_back(std::make_pair("particleSystem", particleMap));
	m_MappedResources.push_back(std::make_pair("spell", spellMap));
	m_MappedResources.push_back(std::make_pair("animation", animationMap));
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
	
	if (p_ResourceType == "model")
	{
		return "assets/models/" + p_ResourceName + ".btx";
	}
	else if (p_ResourceType == "volume")
	{
		return "assets/volumes/CB_" + p_ResourceName + ".txc";
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