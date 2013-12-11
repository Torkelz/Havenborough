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
public:
	struct Resource
	{
		int m_ID;
		string m_Name;
		string m_Path;
		int m_Count;
	};

	vector<Resource> m_LoadedResources;
	
	std::function<bool(const char*, const char*)> m_Create;
	std::function<bool(const char*)> m_Release;
private:
	string m_Type;
public:
	/**
	 * Set resource type.
	 * @param p_Type type to be set
	 */
	void setType(string p_Type);

	/**
	 * Gets the type this resource represents.
	 * @return the type of the resource.
	 */
	string getType();
public:

};


class ResourceManager
{
public:
protected:
	unsigned int m_NextID;
	vector<ResourceType> m_ResourceList;
	ResourceTranslator m_ResourceTranslator;
	boost::filesystem::path m_ProjectDirectory;


public:
	ResourceManager();
	~ResourceManager();

	
	/**
	 * Registers a new resource type with associated create and release fucntions. 
	 * @param p_Type Resource type identifier
	 * @param p_CreateFunc a function pointer which calls the appropiate create function for the resource
	 * @param p_ReleaseFunc a function pointer which calls the appropiate release function for the resource
	 * @return true if new type is added, false if the type already exists
	 */
	bool registerFunction(string p_Type, std::function<bool(const char*, const char*)> p_CreateFunc, std::function<bool(const char*)> p_ReleaseFunc);
	
	/**
	 * Loads a resource.
	 * @param p_ResourceType type of resource
	 * @param p_ResourceName name of the resource
	 * @returns a unique ID for each created resource
	 */
	int loadResource(string p_ResourceType, string p_ResourceName);
	
	/**
	 * Loads a texture, should only be used as callback.
	 * @param p_ResourceName type of resource
	 * @param p_FilaPath absolute file path to the texture to be loaded
	 * @param p_UserData user defined data
	 */
	static void loadModelTexture(const char *p_ResourceName, const char *p_FilePath, void* p_Userdata);
	
	/**
	 * Releases a resource.
	 * @param p_ID ID for the resource to be released
	 * @return true if the resource was released otherwise false.
	 */
	bool releaseResource(int p_ID);
	
private:
	int loadModelTextureImpl(const char *p_ResourceName, const char *p_FilePath);
};

