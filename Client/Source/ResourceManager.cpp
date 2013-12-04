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

bool ResourceManager::loadResource(std::string p_ResourceType, std::string p_ResourceName)
{
	string tempFilePath, temp;
	int tempID;

	temp = m_ResourceTranslator->translate(p_ResourceType, p_ResourceName);
	// check temp if filepath is valid.
	if(temp != "")
		return false;
	if(p_ResourceType == "model")
	{
		tempFilePath = m_ProjectDirectory + " \\ " + temp;

		string tempID = "model" + p_ResourceName;

		if(!m_Graphics->createModel(tempID.c_str(), tempFilePath.c_str()))
			return false;
		
		
	}
	addResourceToList(p_ResourceType, p_ResourceName, tempID, tempFilePath);
	return true;
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