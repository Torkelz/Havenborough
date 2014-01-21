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
		if(key == "*ObjectHeader*")
		{
			readHeader(p_Input);
			std::getline(p_Input, line);
		}
		if(key == "*LightHeader*")
		{
			readLightHeader(p_Input);
			std::getline(p_Input, line);
		}
		if(key == "*CheckPointHeader*")
		{
			readCheckPointHeader(p_Input);
			std::getline(p_Input, line);
		}
		if(key == "#MESH:")
		{
			readMeshList(p_Input);
			std::getline(p_Input, line);
		}
		if(key == "#Light:")
		{
			readLightList(p_Input);
			std::getline(p_Input, line);
		}
	}
}

void LevelLoader::readHeader(std::istream& p_Input)
{
	std::string key, line;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> m_Header.m_NumberOfModels;
}

void LevelLoader::readLightHeader(std::istream& p_Input)
{
	std::string key, line;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> m_Header.m_NumberOfLights;
}

void LevelLoader::readCheckPointHeader(std::istream& p_Input)
{
	std::string key, line;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> m_Header.m_NumberOfCheckPoints;
}

void LevelLoader::readMeshList(std::istream& p_Input)
{
	std::string key, filler, line;
	ModelStruct tempLevel;
	DirectX::XMFLOAT3 tempFlaot3;
	tempLevel = ModelStruct();
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

void LevelLoader::readLightList(std::istream& p_Input)
{
	std::string key, filler, line;
	LightStruct tempLight;
	DirectX::XMFLOAT3 tempFlaot3;
	std::string tempString;
	tempLight = LightStruct();
	m_Stringstream >> tempLight.m_LightName;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
	tempLight.m_Translation = tempFlaot3;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
	tempLight.m_Color = tempFlaot3;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempString;
	if(tempString == "kDirectionalLight")
	{
		tempLight.m_Type = 0;
		DirectionalLight tempDirectional;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempDirectional.m_Intensity;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempDirectional.m_Direction.x >> tempDirectional.m_Direction.y >> tempDirectional.m_Direction.z;
		m_LevelDirectionalLightList.push_back(std::make_pair(tempLight,tempDirectional));
		return;
	}
	if(tempString == "kPointLight")
	{
		tempLight.m_Type = 0;
		PointLight tempDirectional;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempDirectional.m_Intensity;
		m_LevelPointLightList.push_back(std::make_pair(tempLight,tempDirectional));
		return;
	}
	if(tempString == "kSpotLight")
	{
		tempLight.m_Type = 0;
		SpotLight tempDirectional;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempDirectional.m_Intensity;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempDirectional.m_Direction.x >> tempDirectional.m_Direction.y >> tempDirectional.m_Direction.z;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempDirectional.m_ConeAngle;
		m_LevelSpotLightList.push_back(std::make_pair(tempLight,tempDirectional));
		return;
	}
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

const std::vector<LevelLoader::ModelStruct>& LevelLoader::getLevelModelList()
{
	return m_LevelModelList;
}