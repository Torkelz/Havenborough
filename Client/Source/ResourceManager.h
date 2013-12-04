#pragma once

#include "ResourceTranslator.h"
#include <vector>
#include "IGraphics.h"

using std::string;
using std::vector;

typedef unsigned int ResourceIndex;

class ResourceManager
{
public:
private:
	ResourceIndex m_NextIndex;
	vector<vector<string>> m_Resources;

	IGraphics* m_Graphics;
	ResourceTranslator* m_ResourceTranslator;
	string m_ProjectDirectory;

public:
	ResourceManager(IGraphics* p_Graphics, string p_ProjectDirectory);
	~ResourceManager();

	void loadResource(string p_ResourceType, string p_ResourceName);
private:
	void addResourceToList(string p_ResourceType, string p_ResourceName, unsigned int p_ID, string p_FilePath);
	ResourceIndex getNextIndex();
};