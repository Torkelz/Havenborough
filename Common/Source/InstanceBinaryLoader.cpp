#include "InstanceBinaryLoader.h"

#include "CommonExceptions.h"

InstanceBinaryLoader::InstanceBinaryLoader()
{
	m_Header.m_NumberOfModels = 0;
	m_Header.m_NumberOfLights = 0;
	m_Header.m_NumberOfCheckPoints = 0;
}

InstanceBinaryLoader::~InstanceBinaryLoader()
{
	clear();
	m_LevelCheckPointList.shrink_to_fit();
	m_LevelData.shrink_to_fit();
	m_LevelDirectionalLightList.shrink_to_fit();
	m_LevelPointLightList.shrink_to_fit();
	m_LevelSpotLightList.shrink_to_fit();
}

void InstanceBinaryLoader::loadBinaryFile(std::string p_FilePath)
{
	m_Input.open(p_FilePath, std::istream::in | std::istream::binary);
	if(!m_Input)
	{
		throw CommonException("Could not read level file: " + p_FilePath, __LINE__, __FILE__);
	}	

	readStreamData(m_Input);
}

void InstanceBinaryLoader::readStreamData(std::istream& p_Input)
{
	m_Header = readHeader(p_Input);
	if(m_Header.m_NumberOfModels != 0)m_LevelData = readLevel(p_Input);
	if(m_Header.m_NumberOfLights != 0)readLevelLighting(p_Input);
	if(m_Header.m_NumberOfCheckPoints != 0)readLevelCheckPoint(p_Input);
}

InstanceBinaryLoader::Header InstanceBinaryLoader::readHeader(std::istream& p_Input)
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

std::vector<InstanceBinaryLoader::ModelData> InstanceBinaryLoader::readLevel(std::istream& p_Input)
{
	std::vector<InstanceBinaryLoader::ModelData> levelData;
	int numberOfDifferentModels;
	byteToInt(p_Input, numberOfDifferentModels);
	for(int i = 0; i < numberOfDifferentModels; i++)
	{
		ModelData data = ModelData();
		data.m_Animated = false;
		data.m_Transparent = false;
		data.m_CollideAble = false;
		int size;
		byteToString(p_Input, data.m_MeshName);
		byteToBool(p_Input, data.m_Animated);
		byteToBool(p_Input, data.m_Transparent);
		byteToBool(p_Input, data.m_CollideAble);
		byteToInt(p_Input, size);
		data.m_Translation.resize(size);
		p_Input.read(reinterpret_cast<char*>(data.m_Translation.data()),sizeof(DirectX::XMFLOAT3) * size);
		byteToInt(p_Input, size);
		data.m_Rotation.resize(size);
		p_Input.read(reinterpret_cast<char*>(data.m_Rotation.data()),sizeof(DirectX::XMFLOAT3) * size);
		byteToInt(p_Input, size);
		data.m_Scale.resize(size);
		p_Input.read(reinterpret_cast<char*>(data.m_Scale.data()),sizeof(DirectX::XMFLOAT3) * size);
		levelData.push_back(data);
	}

	return levelData;
}

void InstanceBinaryLoader::readLevelLighting(std::istream& p_Input)
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
			p_Input.read(reinterpret_cast<char*>(m_LevelDirectionalLightList.data()),sizeof(DirectionalLight) * size);
		}
		if(type == 1)
		{
			byteToInt(p_Input, size);
			m_LevelPointLightList.resize(size);
			p_Input.read(reinterpret_cast<char*>(m_LevelPointLightList.data()),sizeof(PointLight) * size);
		}
		if(type == 2)
		{
			byteToInt(p_Input, size);
			m_LevelSpotLightList.resize(size);
			p_Input.read(reinterpret_cast<char*>(m_LevelSpotLightList.data()),sizeof(SpotLight) * size);
		}
	}
}

void InstanceBinaryLoader::readLevelCheckPoint(std::istream& p_Input)
{
	std::vector<InstanceBinaryLoader::CheckPointStruct> loader;
	DirectX::XMFLOAT3 *tempStart = &m_LevelCheckPointStart,*tempEnd = &m_LevelCheckPointEnd;
	p_Input.read(reinterpret_cast<char*>(tempStart), sizeof(DirectX::XMFLOAT3));
	p_Input.read(reinterpret_cast<char*>(tempEnd), sizeof(DirectX::XMFLOAT3));
	int size;
	byteToInt(p_Input, size);
	if(size != 0)
	{
		for(int i = 0; i < size; i++)
		{
			int innerSize;
			byteToInt(p_Input, innerSize);
			loader.resize(innerSize);
			if(innerSize != 0)
			{
				p_Input.read(reinterpret_cast<char*>(loader.data()),sizeof(CheckPointStruct) * innerSize);
				m_LevelCheckPointList.push_back(loader);
			}
			loader.clear();
		}
	}
}

const std::vector<InstanceBinaryLoader::ModelData>& InstanceBinaryLoader::getModelData() const
{
	return m_LevelData;
}

const std::vector<InstanceBinaryLoader::DirectionalLight>& InstanceBinaryLoader::getDirectionalLightData() const
{
	return m_LevelDirectionalLightList;
}

const std::vector<InstanceBinaryLoader::PointLight>& InstanceBinaryLoader::getPointLightData() const
{
	return m_LevelPointLightList;
}

const std::vector<InstanceBinaryLoader::SpotLight>& InstanceBinaryLoader::getSpotLightData() const
{
	return m_LevelSpotLightList;
}

DirectX::XMFLOAT3 InstanceBinaryLoader::getCheckPointStart() const
{
	return m_LevelCheckPointStart;
}

DirectX::XMFLOAT3 InstanceBinaryLoader::getCheckPointEnd() const
{
	return m_LevelCheckPointEnd;
}

const std::vector<std::vector<InstanceBinaryLoader::CheckPointStruct>>& InstanceBinaryLoader::getCheckPointData() const
{
	return m_LevelCheckPointList;
}

std::string InstanceBinaryLoader::getDataStream() 
{
	m_Input.seekg(0, m_Input.end);
	std::streampos size = m_Input.tellg();
	m_Input.seekg(0, m_Input.beg);
	if(size >= 0)
	{
		std::vector<char> buffer((unsigned int)size);
		m_Input.read(buffer.data(), size);
		std::string stream(buffer.begin(), buffer.end());
		return stream;
	}
	m_Input.seekg(0, m_Input.beg);

	return NULL;
}

void InstanceBinaryLoader::byteToString(std::istream& p_Input, std::string& p_Return)
{
	int strLength = 0;
	byteToInt(p_Input, strLength);
	std::vector<char> buffer(strLength);
	p_Input.read( buffer.data(), strLength);
	p_Return = std::string(buffer.data(), strLength);
}

void InstanceBinaryLoader::byteToInt(std::istream& p_Input, int& p_Return)
{
	p_Input.read((char*)&p_Return, sizeof(int));
}

void InstanceBinaryLoader::byteToBool(std::istream& p_Input, bool& p_Return)
{
	p_Return = false;
	int tempBool;
	byteToInt(p_Input, tempBool);
	if(tempBool == 1)
	{
		p_Return = true;
	}
}

void InstanceBinaryLoader::clear()
{
	m_LevelCheckPointList.clear();
	m_LevelData.clear();
	m_LevelDirectionalLightList.clear();
	m_LevelPointLightList.clear();
	m_LevelSpotLightList.clear();
	m_Input.close();
}