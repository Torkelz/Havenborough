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
	readHeader(&input);
	m_LevelData = readLevel(&input);
	return true;
}

LevelBinaryLoader::Header LevelBinaryLoader::readHeader(std::istream* p_Input)
{
	Header header;
	byteToInt(p_Input, header.m_NumberOfModels); 
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

const std::vector<LevelBinaryLoader::ModelData>& LevelBinaryLoader::getModelData()
{
	return m_LevelData;
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