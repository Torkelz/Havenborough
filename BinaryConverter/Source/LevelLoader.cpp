#include "LevelLoader.h"

LevelLoader::LevelLoader()
{
	m_Header.m_NumberOfModels = 0;
	m_Header.m_NumberOfLights = 0;
	m_Header.m_NumberOfCheckPoints = 0;
}

LevelLoader::~LevelLoader()
{
	clear();
}

void LevelLoader::clear()
{
	m_LevelModelList.clear();
	m_LevelModelList.shrink_to_fit();
	m_CheckPointStart = DirectX::XMFLOAT3(0, 0, 0);
	m_CheckPointEnd = DirectX::XMFLOAT3(0, 0, 0);
	m_LevelCheckPointList.clear();
	m_LevelCheckPointList.shrink_to_fit();
	m_LevelDirectionalLightList.clear();
	m_LevelDirectionalLightList.shrink_to_fit();
	m_LevelPointLightList.clear();
	m_LevelPointLightList.shrink_to_fit();
	m_LevelSpotLightList.clear();
	m_LevelSpotLightList.shrink_to_fit();
	m_Header.m_NumberOfModels = 0;
	m_Header.m_NumberOfLights = 0;
	m_Header.m_NumberOfCheckPoints = 0;
	m_Stringstream.clear();
	m_ModelHeaders.clear();
	m_ModelHeaders.shrink_to_fit();
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
	readModelHeaders(p_FilePath);
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
			m_Header.m_NumberOfModels = readHeader(p_Input);
			std::getline(p_Input, line);
		}
		else if(key == "*LightHeader*")
		{
			m_Header.m_NumberOfLights = readHeader(p_Input);
			std::getline(p_Input, line);
		}
		else if(key == "*CheckPointHeader*")
		{
			m_Header.m_NumberOfCheckPoints = readHeader(p_Input);
			std::getline(p_Input, line);
		}
		else if(key == "#MESH:")
		{
			readMeshList(p_Input);
			std::getline(p_Input, line);
		}
		else if(key == "#Light:")
		{
			readLightList(p_Input);
			std::getline(p_Input, line);
		}
		else if(key == "#Type:")
		{
			readCheckPointList(p_Input);
			std::getline(p_Input, line);
		}
	}
}

int LevelLoader::readHeader(std::istream& p_Input)
{
	std::string key, line;
	int result;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> result;
	return result;
}

void LevelLoader::readModelHeaders(std::string p_FilePath)
{
	std::ifstream headerFile(getPath(p_FilePath), std::ifstream::binary);
	ModelHeader tempHeader;
	headerFile.seekg(0,std::ifstream::end);
	int size = headerFile.tellg();
	headerFile.seekg(0,std::ifstream::beg);
	while(headerFile.tellg() < size)
	{
		byteToString(headerFile, tempHeader.m_MeshName);
		byteToInt(headerFile, tempHeader.m_Animated);
		byteToInt(headerFile, tempHeader.m_Transparency);
		byteToInt(headerFile, tempHeader.m_Collidable);
		m_ModelHeaders.push_back(tempHeader);
	}
	headerFile.close();
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
		m_Stringstream >> filler >> tempCheckPoint.m_Translation.x >> tempCheckPoint.m_Translation.y >> tempCheckPoint.m_Translation.z;
		m_LevelCheckPointList.push_back(tempCheckPoint);
	}
}

void LevelLoader::byteToString(std::istream& p_Input, std::string& p_Return)
{
	int strLength = 0;
	byteToInt(p_Input, strLength);
	std::vector<char> buffer(strLength);
	p_Input.read( buffer.data(), strLength);
	p_Return = std::string(buffer.data(), strLength);
}

void LevelLoader::byteToInt(std::istream& p_Input, int& p_Return)
{
	p_Input.read((char*)&p_Return, sizeof(int));
}

std::string LevelLoader::getPath(std::string p_FilePath)
{
	std::string file("ModelHeader.txx");
	std::vector<char> buffer(p_FilePath.begin(), p_FilePath.end());
	buffer.push_back(0);
	char *tmp, *type = nullptr;
	tmp = strtok(buffer.data(), "\\");
	while(tmp != nullptr)
	{
		type = tmp;
		tmp = strtok(NULL,"\\");
	}
	int length = buffer.size();
	int size = strlen(type)+1;

	std::string temp;
	temp.append(p_FilePath.data(), length-size);
	temp.append(file.data(),file.size());
	temp.push_back(0);
	return temp;
}

void LevelLoader::clearData()
{
	m_LevelModelList.clear();
	m_CheckPointStart = DirectX::XMFLOAT3(0, 0, 0);
	m_CheckPointEnd = DirectX::XMFLOAT3(0, 0, 0);
	m_LevelCheckPointList.clear();
	m_LevelDirectionalLightList.clear();
	m_LevelPointLightList.clear();
	m_LevelSpotLightList.clear();
	m_Header.m_NumberOfModels = 0;
	m_Header.m_NumberOfLights = 0;
	m_Header.m_NumberOfCheckPoints = 0;
	m_ModelHeaders.clear();
	m_Stringstream.clear();
}

LevelLoader::LevelHeader LevelLoader::getLevelHeader()
{
	return m_Header;
}

const std::vector<LevelLoader::ModelStruct>& LevelLoader::getLevelModelList() const
{
	return m_LevelModelList;
}

const std::vector<std::pair<LevelLoader::LightData, LevelLoader::DirectionalLight>>& LevelLoader::getLevelDirectionalLightList() const
{
	return m_LevelDirectionalLightList;
}

const std::vector<std::pair<LevelLoader::LightData, LevelLoader::PointLight>>& LevelLoader::getLevelPointLightList() const
{
	return m_LevelPointLightList;
}

const std::vector<std::pair<LevelLoader::LightData, LevelLoader::SpotLight>>& LevelLoader::getLevelSpotLightList() const
{
	return m_LevelSpotLightList;
}

const std::vector<LevelLoader::CheckPointStruct>& LevelLoader::getLevelCheckPointList() const
{
	return m_LevelCheckPointList;
}

DirectX::XMFLOAT3 LevelLoader::getLevelCheckPointStart() const
{
	return m_CheckPointStart;
}

DirectX::XMFLOAT3 LevelLoader::getLevelCheckPointEnd() const
{
	return m_CheckPointEnd;
}

const std::vector<LevelLoader::ModelHeader> LevelLoader::getModelInformation() const
{
	return m_ModelHeaders;
}