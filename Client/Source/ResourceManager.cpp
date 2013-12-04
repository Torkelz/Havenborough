#include "ResourceManager.h"

ResourceManager::ResourceManager(IGraphics* p_Graphics, string p_ProjectDirectory)
{
	m_NextIndex = 0;

	m_Graphics = p_Graphics;
	m_ProjectDirectory = p_ProjectDirectory;
	m_ResourceTranslator = new ResourceTranslator();
}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::loadResource(std::string p_ResourceType, std::string p_ResourceName)
{
	string tempFilePath, temp;
	int tempID;

	temp = m_ResourceTranslator->translate(p_ResourceType, p_ResourceName);
	// check tempfilepath if filepath is valid.

	tempFilePath = m_ProjectDirectory + " \\ " + temp;

	m_Graphics->createModel(p_ResourceName.c_str(), tempFilePath.c_str());

	addResourceToList(p_ResourceType, p_ResourceName, tempID, tempFilePath);
}

void ResourceManager::addResourceToList(string p_ResourceType, string p_ResourceName, unsigned int p_ID, string p_FilePath)
{
	std::vector<string> tempResource;

	tempResource.push_back(p_ResourceType);
	tempResource.push_back(p_ResourceName);
	tempResource.push_back(std::to_string(p_ID));
	tempResource.push_back(p_FilePath);

	m_Resources.push_back(tempResource);
}

ResourceIndex ResourceManager::getNextIndex()
{
	return m_NextIndex++;
}