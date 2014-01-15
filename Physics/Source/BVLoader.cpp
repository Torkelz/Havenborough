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
	//std::ifstream input(p_FilePath, std::istream::in | std::istream::binary);
	std::ifstream input(p_FilePath, std::istream::in);
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
	std::string line;
	/*byteToString(p_Input, tempHeader.m_modelName);
	byteToInt(p_Input, tempHeader.m_numMaterial);
	byteToInt(p_Input, tempHeader.m_numVertex);
	byteToInt(p_Input, tempHeader.m_numMaterialBuffer);
	byteToInt(p_Input, tempHeader.m_numJoints);
	byteToInt(p_Input, tempHeader.m_numFrames);*/
	std::getline(*p_Input, line);
	std::string temp;
	std::getline(*p_Input, line);
	std::stringstream ss(line);
	ss >> temp >> tempHeader.m_numMaterial;
	std::getline(*p_Input, line);
	ss = std::stringstream(line);
	ss >> temp >> tempHeader.m_modelName;
	std::getline(*p_Input, line);
	ss = std::stringstream(line);
	ss >> temp >> tempHeader.m_numVertex;
	std::getline(*p_Input, line);
	ss = std::stringstream(line);
	ss >> temp >> tempHeader.m_numFrames;

	return tempHeader;
}


std::vector<BVLoader::BoundingVolume> BVLoader::readBoundingVolume(int p_NumberOfVertex, std::istream* p_Input)
{
	std::vector<BoundingVolume> boundingVolume;//(p_NumberOfVertex);
	std::vector<DirectX::XMFLOAT4> tempVertices;
	std::vector<DirectX::XMFLOAT3> tempFaces;
	//p_Input->read(reinterpret_cast<char*>(boundingVolume.data()), sizeof(boundingVolume) * p_NumberOfVertex);
	std::string line, temp;
	std::stringstream ss;
	std::getline(*p_Input, line);
	std::getline(*p_Input, line);
	for(int i = 0; i < m_FileHeader.m_numVertex;i++)
	{
		DirectX::XMFLOAT4 tv;
		std::getline(*p_Input, line);
		ss = std::stringstream(line);
		ss >> temp >> tv.x >> tv.y >> tv.z;
		tv.w = 1.0f;

		tempVertices.push_back(tv);
	}

	std::getline(*p_Input, line);
	std::getline(*p_Input, line);
	std::getline(*p_Input, line);
	std::getline(*p_Input, line);

	for(int i = 0; i < m_FileHeader.m_numFrames; i++)
	{
		std::getline(*p_Input, line);
		ss = std::stringstream(line);
		DirectX::XMFLOAT3 tempFace;
		ss >> tempFace.x >> temp >> tempFace.y >> temp >> tempFace.z;

		tempFaces.push_back(tempFace);
	}

	for(int i = 0; i < m_FileHeader.m_numFrames; i++)
	{
		BoundingVolume tempBV;

		tempBV.m_Postition = tempVertices[(unsigned int)tempFaces[i].x];
		boundingVolume.push_back(tempBV);
		tempBV.m_Postition = tempVertices[(unsigned int)tempFaces[i].y];
		boundingVolume.push_back(tempBV);
		tempBV.m_Postition = tempVertices[(unsigned int)tempFaces[i].z];
		boundingVolume.push_back(tempBV);
	}

	boundingVolume.shrink_to_fit();
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