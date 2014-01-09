#include "BVLoader.h"


BVLoader::BVLoader(void)
{
}


BVLoader::~BVLoader(void)
{
	clear();
}

void BVLoader::clear()
{
	m_BoundingVolume.clear();
	m_BoundingVolume.shrink_to_fit();
}

bool BVLoader::loadBinaryFile(std::string p_FilePath)
{
	clearData();
	std::ifstream input(p_FilePath, std::istream::in | std::istream::binary);
	if(!input)
	{
		return false;
	}
	m_FileHeader = readHeader(&input);
	if(m_FileHeader.m_numMaterial != 0)
	{
		//return false;
	}
	if(m_FileHeader.m_numVertex > 0)
	{
		m_BoundingVolume = readBoundingVolume(m_FileHeader.m_numVertex, &input);
	}
	else
	{
		return false;
	}
	
	return true;
}

BVLoader::Header BVLoader::readHeader(std::istream* p_Input)
{
	Header tempHeader;
	byteToString(p_Input, tempHeader.m_modelName);
	byteToInt(p_Input, tempHeader.m_numMaterial);
	byteToInt(p_Input, tempHeader.m_numVertex);
	byteToInt(p_Input, tempHeader.m_numMaterialBuffer);
	byteToInt(p_Input, tempHeader.m_numJoints);
	byteToInt(p_Input, tempHeader.m_numFrames);
	return tempHeader;
}


std::vector<BVLoader::BoundingVolume> BVLoader::readBoundingVolume(int p_NumberOfVertex, std::istream* p_Input)
{
	std::vector<BoundingVolume> boundingVolume(p_NumberOfVertex);
	p_Input->read(reinterpret_cast<char*>(boundingVolume.data()), sizeof(boundingVolume) * p_NumberOfVertex);
	return boundingVolume;
}

void BVLoader::byteToString(std::istream* p_Input, std::string& p_Return)
{
	int strLength = 0;
	byteToInt(p_Input, strLength);
	std::vector<char> buffer(strLength);
	p_Input->read( buffer.data(), strLength);
	p_Return = std::string(buffer.data(), strLength);
}

void BVLoader::byteToInt(std::istream* p_Input, int& p_Return)
{
	p_Input->read((char*)&p_Return, sizeof(int));
}

BVLoader::Header BVLoader::getLevelHeader()
{
	return m_FileHeader;
}

const std::vector<BVLoader::BoundingVolume>& BVLoader::getBoundingVolumes()
{
	return m_BoundingVolume;
}

void BVLoader::clearData()
{
	m_FileHeader.m_modelName = "";
	m_FileHeader.m_numFrames = 0;
	m_FileHeader.m_numJoints = 0;
	m_FileHeader.m_numMaterial = 0;
	m_FileHeader.m_numMaterialBuffer = 0;
	m_FileHeader.m_numVertex = 0;
	m_BoundingVolume.clear();
}