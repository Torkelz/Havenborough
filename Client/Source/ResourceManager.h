#pragma once
#pragma once
#include <algorithm>
#include <functional>
#include <vector>
#include "ResourceTranslator.h"
#include "MyExceptions.h"

using std::string;
using std::vector;

class Resource
{
private:
	string m_Type;
public:
	void setType(string p_Type);
	string getType();
	vector<std::pair<int, string>> m_LoadedResources;
	
	std::function<bool(const char*, const char*)> m_Create;
	std::function<bool(const char*)> m_Release;
};


class ResourceManager
{
public:
private:
	unsigned int m_NextID;
	vector<Resource> m_ResourceList;
	ResourceTranslator m_ResourceTranslator;
	string m_ProjectDirectory;

public:
	ResourceManager(string p_ProjectDirectory);
	~ResourceManager();

	bool registerFunction(string p_Type, std::function<bool(const char*, const char*)> p_CreateFunc, std::function<bool(const char*)> p_ReleaseFunc);
	int loadResource(string p_ResourceType, string p_ResourceName);
	bool releaseResource(int p_ID);
};

