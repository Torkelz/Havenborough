#include "ResourceManager.h"
#if defined(_DEBUG)
#define DEBUG true
#else
#define DEBUG false
#endif

//loader.reg("model", std::bind(m_Graphic->createModel))

void Resource::setType(string p_Type)
{
	m_Type = p_Type;
}
string Resource::getType()
{
	return m_Type;
}

ResourceManager::ResourceManager(string p_ProjectDirectory)
{
	m_ProjectDirectory = p_ProjectDirectory;

	if(DEBUG)
	{
		m_ProjectDirectory.erase(m_ProjectDirectory.end()-16, m_ProjectDirectory.end());
		m_ProjectDirectory = m_ProjectDirectory + "Bin\\";
	}
	else
	{
		m_ProjectDirectory.erase(m_ProjectDirectory.end()-10, m_ProjectDirectory.end());
	}

	m_ResourceTranslator = new ResourceTranslator();
	NumberOfResources = 0;
}

ResourceManager::~ResourceManager()
{

}

bool ResourceManager::registerFunction(string p_Type, std::function<bool(const char*, const char*)> p_CreateFunc, std::function<bool(const char*)> p_ReleaseFunc)
{
	for(auto &rl : m_ResourceList)
	{
		if(rl.getType() == p_Type)
		{
			return false;
		}
	}

	Resource temp;
	temp.setType(p_Type);
	temp.m_Create = p_CreateFunc;
	temp.m_Release = p_ReleaseFunc;
	m_ResourceList.push_back(temp);
	return true;
}


int ResourceManager::loadResource(string p_ResourceType, string p_ResourceName)
{
	string tempFilePath, temp;
	int id = -1;
	for(auto &rl : m_ResourceList)
	{
		if(rl.getType() == p_ResourceType)
		{

			for(auto &t : rl.m_LoadedResources)
			{
				if(p_ResourceName == t.second)
				{
					return t.first;
				}
			}

			tempFilePath = m_ResourceTranslator->translate(p_ResourceType, p_ResourceName);
			tempFilePath = m_ProjectDirectory + tempFilePath;

			if(rl.m_Create(p_ResourceName.c_str(), tempFilePath.c_str()))
			{
				std::pair<int, string> newRes;
				newRes.second = p_ResourceName;
				newRes.first = NumberOfResources++;
				rl.m_LoadedResources.push_back(newRes);
			}
		}
		else
		{
			return -1;
		}
	}
	return id;
}

void ResourceManager::releaseResource(int p_ID)
{
	int tempID = -1;
	for(auto &rl : m_ResourceList)
	{
		for(auto &t : rl.m_LoadedResources)
		{
			if(t.first == p_ID)
			{

			}
		}

		for(auto &t : rl.m_LoadedResources)
		{
			
		}
	}
}
