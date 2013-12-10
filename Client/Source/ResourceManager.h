#pragma once

#include <algorithm>
#include <functional>
#include <vector>
#include "ResourceTranslator.h"
#include "MyExceptions.h"

#include <boost/filesystem.hpp>

using std::string;
using std::vector;

class ResourceType
{
private:
	string m_Type;
public:
	struct Resource
	{
		int m_ID;
		string m_Name;
		string m_Path;
		int m_Count;
	};

	void setType(string p_Type);
	string getType();
	vector<Resource> m_LoadedResources;
	
	std::function<bool(const char*, const char*)> m_Create;
	std::function<bool(const char*)> m_Release;
};


class ResourceManager
{
public:
private:
	unsigned int m_NextID;
	vector<ResourceType> m_ResourceList;
	ResourceTranslator m_ResourceTranslator;
	boost::filesystem::path m_ProjectDirectory;

public:
	ResourceManager(string p_ProjectDirectory);
	~ResourceManager();

	bool registerFunction(string p_Type, std::function<bool(const char*, const char*)> p_CreateFunc, std::function<bool(const char*)> p_ReleaseFunc);
	int loadResource(string p_ResourceType, string p_ResourceName);
	bool releaseResource(int p_ID);
};

