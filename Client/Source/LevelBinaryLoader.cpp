#include "LevelBinaryLoader.h"

LevelBinaryLoader::LevelBinaryLoader()
{

}

LevelBinaryLoader::~LevelBinaryLoader()
{

}

bool LevelBinaryLoader::loadBinaryFile(std::string p_FilePath)
{
	std::ifstream input(p_FilePath, std::istream::in | std::istream::binary);
	if(!input)
	{
		return false;
	}	
	m_Header = readHeader(&input);
	m_LevelData = readLevel(&input);
	readLevelLighting(&input);
	readLevelCheckPoint(&input);
	input.close();
	return true;
}

LevelBinaryLoader::Header LevelBinaryLoader::readHeader(std::istream* p_Input)
{
	Header header;
	byteToInt(p_Input, header.m_NumberOfModels); 
	byteToInt(p_Input, header.m_NumberOfLights);
	byteToInt(p_Input, header.m_NumberOfCheckPoints);
	return header;
}

std::vector<LevelBinaryLoader::ModelData> LevelBinaryLoader::readLevel(std::istream* p_Input)
{
	std::vector<LevelBinaryLoader::ModelData> levelData;
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

void LevelBinaryLoader::readLevelLighting(std::istream* p_Input)
{
	int type,size;
	for(int i = 0 ; i < 3; i++)
	{
		byteToInt(p_Input, type);
	
		if(type == 0)
		{
			byteToInt(p_Input, size);
			m_LevelDirectionalLightList.resize(size);
			p_Input->read(reinterpret_cast<char*>(m_LevelDirectionalLightList.data()),sizeof(std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::DirectionalLight>) * size);
		}
		if(type == 1)
		{
			byteToInt(p_Input, size);
			m_LevelPointLightList.resize(size);
			p_Input->read(reinterpret_cast<char*>(m_LevelPointLightList.data()),sizeof(std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::PointLight>) * size);
		}
		if(type == 2)
		{
			byteToInt(p_Input, size);
			m_LevelSpotLightList.resize(size);
			p_Input->read(reinterpret_cast<char*>(m_LevelSpotLightList.data()),sizeof(std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::SpotLight>) * size);
		}
	}
}

void LevelBinaryLoader::readLevelCheckPoint(std::istream* p_Input)
{
	DirectX::XMFLOAT3 *tempStart = &m_LevelCheckPointStart,*tempEnd = &m_LevelCheckPointEnd;
	p_Input->read(reinterpret_cast<char*>(tempStart), sizeof(DirectX::XMFLOAT3));
	p_Input->read(reinterpret_cast<char*>(tempEnd), sizeof(DirectX::XMFLOAT3));
	int size;
	byteToInt(p_Input, size);
	m_LevelCheckPointList.resize(size);
	p_Input->read(reinterpret_cast<char*>(m_LevelCheckPointList.data()), sizeof(LevelBinaryLoader::CheckPointStruct) * size);
}

const std::vector<LevelBinaryLoader::ModelData>& LevelBinaryLoader::getModelData()
{
	return m_LevelData;
}

const std::vector<std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::DirectionalLight>>& LevelBinaryLoader::getDirectionalLightData()
{
	return m_LevelDirectionalLightList;
}

const std::vector<std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::PointLight>>& LevelBinaryLoader::getPointLightData()
{
	return m_LevelPointLightList;
}

const std::vector<std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::SpotLight>>& LevelBinaryLoader::getSpotLightData()
{
	return m_LevelSpotLightList;
}

DirectX::XMFLOAT3 LevelBinaryLoader::getCheckPointStart()
{
	return m_LevelCheckPointStart;
}

DirectX::XMFLOAT3 LevelBinaryLoader::getCheckPointEnd()
{
	return m_LevelCheckPointEnd;
}

const std::vector<LevelBinaryLoader::CheckPointStruct>& LevelBinaryLoader::getCheckPointData()
{
	return m_LevelCheckPointList;
}

void LevelBinaryLoader::byteToString(std::istream* p_Input, std::string& p_Return)
{
	int strLength = 0;
	byteToInt(p_Input, strLength);
	std::vector<char> buffer(strLength);
	p_Input->read( buffer.data(), strLength);
	p_Return = std::string(buffer.data(), strLength);
}

void LevelBinaryLoader::byteToInt(std::istream* p_Input, int& p_Return)
{
	p_Input->read((char*)&p_Return, sizeof(int));
}