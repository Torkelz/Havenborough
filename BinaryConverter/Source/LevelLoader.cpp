#include "LevelLoader.h"

LevelLoader::LevelLoader()
{

}

LevelLoader::~LevelLoader()
{
	clear();
}

void LevelLoader::clear()
{
	m_LevelModelList.clear();
	m_LevelModelList.shrink_to_fit();
	m_Header.m_NumberOfModels = 0;
	m_Stringstream.clear();
}

bool LevelLoader::loadLevel(std::string p_FilePath)
{
	std::ifstream input(p_FilePath, std::ifstream::in);
	if(!input)
	{
		return false;
	}
	clearData();
	startReading(input);

	input.close();

	return true;
}

void LevelLoader::startReading(std::istream& p_Input)
{
	std::string line, key, filler;
	while(!p_Input.eof() && std::getline(p_Input, line))
	{
		key = "";
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> key >> std::ws;
		if(key == "*Header")
		{
			createHeader(p_Input);
		}

		if(key == "#MESH")
		{
			createMeshList(p_Input);
		}
	}
}

void LevelLoader::createHeader(std::istream& p_Input)
{
	std::string key, line;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> m_Header.m_NumberOfModels;
}

void LevelLoader::createMeshList(std::istream& p_Input)
{
	std::string key, filler, line;
	LevelStruct tempLevel;
	DirectX::XMFLOAT3 tempFlaot3;
	tempLevel = LevelStruct();
	m_Stringstream >> tempLevel.m_MeshName;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
	tempLevel.m_Translation = tempFlaot3;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
	tempLevel.m_Rotation = tempFlaot3;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
	tempLevel.m_Scale = tempFlaot3;

	m_LevelModelList.push_back(tempLevel);
}

void LevelLoader::clearData()
{
	m_LevelModelList.clear();
	m_Header.m_NumberOfModels = 0;
	m_Stringstream.clear();
}

LevelLoader::LevelHeader LevelLoader::getLevelHeader()
{
	return m_Header;
}

const std::vector<LevelLoader::LevelStruct>& LevelLoader::getLevelModelList()
{
	return m_LevelModelList;
}