#include "ModelLoader.h"

ModelLoader::ModelLoader()
{
	m_Pos = nullptr;
	m_Tangents = nullptr;
	m_Normals = nullptr;
	m_Texture = nullptr;
}

ModelLoader::~ModelLoader()
{
	m_Pos = nullptr;
	m_Tangents = nullptr;
	m_Normals = nullptr;
	m_Texture = nullptr;
}

bool ModelLoader::loadFile(std::string p_Filename)
{
	std::ifstream input((p_Filename.c_str()));
	std::string buffer;

	if(!input)
	{
		return false;
	}

	while ( std::getline(input, buffer))
	{
		std::istringstream line(buffer);
		std::string line_t;

		if(buffer.find("Position") == 0);
	}
}