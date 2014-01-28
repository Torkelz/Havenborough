#include "LevelStreamReader.h"

LevelStreamReader::LevelStreamReader()
{
	m_Header.m_NumberOfModels = 0;
	m_Header.m_NumberOfLights = 0;
	m_Header.m_NumberOfCheckPoints = 0;
}

LevelStreamReader::~LevelStreamReader()
{
	clear();
	m_LevelCheckPointList.shrink_to_fit();
	m_LevelData.shrink_to_fit();
	m_LevelDirectionalLightList.shrink_to_fit();
	m_LevelPointLightList.shrink_to_fit();
	m_LevelSpotLightList.shrink_to_fit();
}

void LevelStreamReader::readStreamData(std::istream* p_Input)
{
	m_Header = readHeader(p_Input);
	if(m_Header.m_NumberOfModels != 0)m_LevelData = readLevel(p_Input);
	if(m_Header.m_NumberOfLights != 0)readLevelLighting(p_Input);
	if(m_Header.m_NumberOfCheckPoints != 0)readLevelCheckPoint(p_Input);
}

LevelStreamReader::Header LevelStreamReader::readHeader(std::istream* p_Input)
{
	Header header;
	header.m_NumberOfModels = 0;
	header.m_NumberOfLights = 0;
	header.m_NumberOfCheckPoints = 0;
	byteToInt(p_Input, header.m_NumberOfModels); 
	byteToInt(p_Input, header.m_NumberOfLights);
	byteToInt(p_Input, header.m_NumberOfCheckPoints);
	return header;
}

std::vector<LevelStreamReader::ModelData> LevelStreamReader::readLevel(std::istream* p_Input)
{
	std::vector<LevelStreamReader::ModelData> levelData;
	int numberOfDifferentModels;
	byteToInt(p_Input, numberOfDifferentModels);
	for(int i = 0; i < numberOfDifferentModels; i++)
	{
		ModelData data = ModelData();
		int size;
		byteToString(p_Input, data.m_MeshName);
		byteToInt(p_Input, size);
		data.m_Translation.resize(size);
		p_Input->read(reinterpret_cast<char*>(data.m_Translation.data()),sizeof(DirectX::XMFLOAT3) * size);
		byteToInt(p_Input, size);
		data.m_Rotation.resize(size);
		p_Input->read(reinterpret_cast<char*>(data.m_Rotation.data()),sizeof(DirectX::XMFLOAT3) * size);
		byteToInt(p_Input, size);
		data.m_Scale.resize(size);
		p_Input->read(reinterpret_cast<char*>(data.m_Scale.data()),sizeof(DirectX::XMFLOAT3) * size);
		levelData.push_back(data);
	}

	return levelData;
}

void LevelStreamReader::readLevelLighting(std::istream* p_Input)
{
	int type,size, numberOfDifferentLights;
	byteToInt(p_Input, numberOfDifferentLights);
	for(int i = 0 ; i < numberOfDifferentLights; i++)
	{
		byteToInt(p_Input, type);
	
		if(type == 0)
		{
			byteToInt(p_Input, size);
			m_LevelDirectionalLightList.resize(size);
			p_Input->read(reinterpret_cast<char*>(m_LevelDirectionalLightList.data()),sizeof(LevelStreamReader::DirectionalLight) * size);
		}
		if(type == 1)
		{
			byteToInt(p_Input, size);
			m_LevelPointLightList.resize(size);
			p_Input->read(reinterpret_cast<char*>(m_LevelPointLightList.data()),sizeof(LevelStreamReader::PointLight) * size);
		}
		if(type == 2)
		{
			byteToInt(p_Input, size);
			m_LevelSpotLightList.resize(size);
			p_Input->read(reinterpret_cast<char*>(m_LevelSpotLightList.data()),sizeof(LevelStreamReader::SpotLight) * size);
		}
	}
}

void LevelStreamReader::readLevelCheckPoint(std::istream* p_Input)
{
	DirectX::XMFLOAT3 *tempStart = &m_LevelCheckPointStart,*tempEnd = &m_LevelCheckPointEnd;
	p_Input->read(reinterpret_cast<char*>(tempStart), sizeof(DirectX::XMFLOAT3));
	p_Input->read(reinterpret_cast<char*>(tempEnd), sizeof(DirectX::XMFLOAT3));
	int size;
	byteToInt(p_Input, size);
	m_LevelCheckPointList.resize(size);
	p_Input->read(reinterpret_cast<char*>(m_LevelCheckPointList.data()), sizeof(LevelStreamReader::CheckPointStruct) * size);
}

const std::vector<LevelStreamReader::ModelData>& LevelStreamReader::getModelData() const
{
	return m_LevelData;
}

const std::vector<LevelStreamReader::DirectionalLight>& LevelStreamReader::getDirectionalLightData() const
{
	return m_LevelDirectionalLightList;
}

const std::vector<LevelStreamReader::PointLight>& LevelStreamReader::getPointLightData() const
{
	return m_LevelPointLightList;
}

const std::vector<LevelStreamReader::SpotLight>& LevelStreamReader::getSpotLightData() const
{
	return m_LevelSpotLightList;
}

DirectX::XMFLOAT3 LevelStreamReader::getCheckPointStart() const
{
	return m_LevelCheckPointStart;
}

DirectX::XMFLOAT3 LevelStreamReader::getCheckPointEnd() const
{
	return m_LevelCheckPointEnd;
}

const std::vector<LevelStreamReader::CheckPointStruct>& LevelStreamReader::getCheckPointData() const
{
	return m_LevelCheckPointList;
}

void LevelStreamReader::byteToString(std::istream* p_Input, std::string& p_Return)
{
	int strLength = 0;
	byteToInt(p_Input, strLength);
	std::vector<char> buffer(strLength);
	p_Input->read( buffer.data(), strLength);
	p_Return = std::string(buffer.data(), strLength);
}

void LevelStreamReader::byteToInt(std::istream* p_Input, int& p_Return)
{
	p_Input->read((char*)&p_Return, sizeof(int));
}

void LevelStreamReader::clear()
{
	m_LevelCheckPointList.clear();
	m_LevelData.clear();
	m_LevelDirectionalLightList.clear();
	m_LevelPointLightList.clear();
	m_LevelSpotLightList.clear();
}