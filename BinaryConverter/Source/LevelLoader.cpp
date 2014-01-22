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
		if(key == "#Type:")
		{
			readCheckPointList(p_Input);
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
	m_Stringstream >> tempLevel.m_MeshName;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempLevel.m_Translation.x >> tempLevel.m_Translation.y >> tempLevel.m_Translation.z;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempLevel.m_Rotation.x >> tempLevel.m_Rotation.y >> tempLevel.m_Rotation.z;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempLevel.m_Scale.x >> tempLevel.m_Scale.y >> tempLevel.m_Scale.z;

	m_LevelModelList.push_back(tempLevel);
}

void LevelLoader::readLightList(std::istream& p_Input)
{
	std::string key, filler, line, tempName;
	LightData tempLight;
	std::string tempString;
	m_Stringstream >> tempName;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempLight.m_Translation.x >> tempLight.m_Translation.y >> tempLight.m_Translation.z;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> tempLight.m_Color.x >> tempLight.m_Color.y >> tempLight.m_Color.z;
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
		tempLight.m_Type = 1;
		PointLight tempDirectional;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempDirectional.m_Intensity;
		m_LevelPointLightList.push_back(std::make_pair(tempLight,tempDirectional));
		return;
	}
	if(tempString == "kSpotLight")
	{
		tempLight.m_Type = 2;
		SpotLight tempSpot;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempSpot.m_Intensity;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempSpot.m_Direction.x >> tempSpot.m_Direction.y >> tempSpot.m_Direction.z;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempSpot.m_ConeAngle >> tempSpot.m_PenumbraAngle;
		m_LevelSpotLightList.push_back(std::make_pair(tempLight,tempSpot));
		return;
	}
}

void LevelLoader::readCheckPointList(std::istream& p_Input)
{
	std::string key, filler, line;
	CheckPointStruct tempCheckPoint;
	std::string tempString;
	m_Stringstream >> tempString;
	if(tempString == "Start")
	{
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> m_CheckPointStart.x >> m_CheckPointStart.y >> m_CheckPointStart.z;
	}
	else if(tempString == "End")
	{
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> m_CheckPointEnd.x >> m_CheckPointEnd.y >> m_CheckPointEnd.z;
	}
	else
	{
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempCheckPoint.m_Number;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempCheckPoint.m_Transaltion.x >> tempCheckPoint.m_Transaltion.y >> tempCheckPoint.m_Transaltion.z;
		m_LevelCheckPointList.push_back(tempCheckPoint);
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

const std::vector<std::pair<LevelLoader::LightData, LevelLoader::DirectionalLight>>& LevelLoader::getLevelDirectionalLightList()
{
	return m_LevelDirectionalLightList;
}

const std::vector<std::pair<LevelLoader::LightData, LevelLoader::PointLight>>& LevelLoader::getLevelPointLightList()
{
	return m_LevelPointLightList;
}

const std::vector<std::pair<LevelLoader::LightData, LevelLoader::SpotLight>>& LevelLoader::getLevelSpotLightList()
{
	return m_LevelSpotLightList;
}

const std::vector<LevelLoader::CheckPointStruct>& LevelLoader::getLevelCheckPointList()
{
	return m_LevelCheckPointList;
}

DirectX::XMFLOAT3 LevelLoader::getLevelCheckPointStart()
{
	return m_CheckPointStart;
}

DirectX::XMFLOAT3 LevelLoader::getLevelCheckPointEnd()
{
	return m_CheckPointEnd;
}