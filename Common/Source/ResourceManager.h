#pragma once
#include "ResourceTranslator.h"

#include <vector>
#include <string>
#include <boost/filesystem.hpp>

class ResourceType
{
public:
	struct Resource
	{
		int m_ID;
		std::string m_Name;
		std::string m_Path;
		int m_Count;
	};

	std::vector<Resource> m_LoadedResources;
	
	std::function<bool(const char*, const char*)> m_Create;
	std::function<bool(const char*)> m_Release;
private:
	std::string m_Type;
public:
	/**
	 * Set resource type.
	 * @param p_Type type to be set
	 */
	void setType(std::string p_Type);

	/**
	 * Gets the type this resource represents.
	 * @return the type of the resource.
	 */
	std::string getType();
public:

};


class ResourceManager
{
public:
protected:
	unsigned int m_NextID;
	std::vector<ResourceType> m_ResourceList;
	ResourceTranslator m_ResourceTranslator;
	boost::filesystem::path m_ProjectDirectory;


public:
	ResourceManager();
	ResourceManager(const boost::filesystem::path& p_RootPath);
	~ResourceManager();

	
	/**
	 * Registers a new resource type with associated create and release functions. 
	 * @param p_Type Resource type identifier
	 * @param p_CreateFunc a function pointer which calls the appropriate create function for the resource
	 * @param p_ReleaseFunc a function pointer which calls the appropriate release function for the resource
	 * @return true if new type is added, false if the type already exists
	 */
	bool registerFunction(std::string p_Type, std::function<bool(const char*, const char*)> p_CreateFunc,
		std::function<bool(const char*)> p_ReleaseFunc);
	
	/**
	 * Loads a resource.
	 * @param p_ResourceType type of resource
	 * @param p_ResourceName name of the resource
	 * @returns a unique ID for each created resource
	 */
	int loadResource(std::string p_ResourceType, std::string p_ResourceName);
	
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

	static void releaseModelTexture(const char *p_ResournceName, void* p_Userdata);
	
private:
	int loadModelTextureImpl(const char *p_ResourceName, const char *p_FilePath);
	void releaseModelTextureImpl(const char *p_ResourceName);
};

