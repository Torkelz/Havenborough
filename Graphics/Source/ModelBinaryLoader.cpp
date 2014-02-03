#include "ModelBinaryLoader.h"
#include <DirectXMath.h>


ModelBinaryLoader::ModelBinaryLoader()
{

}

ModelBinaryLoader::~ModelBinaryLoader()
{
	clear();
}

void ModelBinaryLoader::clear()
{
	m_Material.clear();
	m_Material.shrink_to_fit();
	m_AnimationVertexBuffer.clear();
	m_AnimationVertexBuffer.shrink_to_fit();
	m_VertexBuffer.clear();
	m_VertexBuffer.shrink_to_fit();
	m_MaterialBuffer.clear();
	m_MaterialBuffer.shrink_to_fit();
}

ModelBinaryLoader::Header ModelBinaryLoader::readHeader(std::istream* p_Input)
{
	Header tempHeader;
	byteToString(p_Input, tempHeader.m_ModelName);
	byteToInt(p_Input, tempHeader.m_NumMaterial);
	byteToInt(p_Input, tempHeader.m_NumVertex);
	byteToInt(p_Input, tempHeader.m_NumMaterialBuffer);
	byteToInt(p_Input, tempHeader.m_NumJoints);
	return tempHeader;
}

std::vector<Material> ModelBinaryLoader::readMaterial(int p_NumberOfMaterial, std::istream* p_Input)
{
	Material temp;
	std::vector<Material> tempVector;
	for(int i = 0; i < p_NumberOfMaterial; i++)
	{
		byteToString(p_Input, temp.m_MaterialID);
		byteToString(p_Input, temp.m_DiffuseMap);
		byteToString(p_Input, temp.m_NormalMap);
		byteToString(p_Input, temp.m_SpecularMap);
		tempVector.push_back(temp);
	}
	return tempVector;
}

std::vector<MaterialBuffer> ModelBinaryLoader::readMaterialBuffer(int p_NumberOfMaterialBuffers, std::istream* p_Input)
{
	MaterialBuffer temp;
	std::vector<MaterialBuffer> tempBuffer;
	for(int i = 0; i < p_NumberOfMaterialBuffers; i++)
	{
		byteToString(p_Input, temp.material);
		byteToInt(p_Input, temp.start);
		byteToInt(p_Input, temp.length);
		tempBuffer.push_back(temp);
	}
	return tempBuffer;
}

std::vector<StaticVertex> ModelBinaryLoader::readVertexBuffer(int p_NumberOfVertex, std::istream* p_Input)
{
	std::vector<StaticVertex> vertexBuffer(p_NumberOfVertex);
	p_Input->read(reinterpret_cast<char*>(vertexBuffer.data()), sizeof(StaticVertex) * p_NumberOfVertex);
	return vertexBuffer;
}

std::vector<AnimatedVertex> ModelBinaryLoader::readVertexBufferAnimation(int p_NumberOfVertex, std::istream* p_Input)
{
	std::vector<AnimatedVertex> vertexBuffer(p_NumberOfVertex);
	p_Input->read(reinterpret_cast<char*>(vertexBuffer.data()), sizeof(AnimatedVertex) * p_NumberOfVertex);
	return vertexBuffer;
}

void ModelBinaryLoader::byteToString(std::istream* p_Input, std::string& p_Return)
{
	int strLength = 0;
	byteToInt(p_Input, strLength);
	std::vector<char> buffer(strLength);
	p_Input->read( buffer.data(), strLength);
	p_Return = std::string(buffer.data(), strLength);
}

void ModelBinaryLoader::byteToInt(std::istream* p_Input, int& p_Return)
{
	p_Input->read((char*)&p_Return, sizeof(int));
}

void ModelBinaryLoader::loadBinaryFile(std::string p_FilePath)
{
	clearData();
	std::ifstream input(p_FilePath, std::istream::in | std::istream::binary);
	if(!input)
	{
		throw;
	}
	m_FileHeader = readHeader(&input);
	m_Material = readMaterial(m_FileHeader.m_NumMaterial,&input);
	if(m_FileHeader.m_NumJoints > 0)
	{
		m_AnimationVertexBuffer = readVertexBufferAnimation(m_FileHeader.m_NumVertex, &input);
	}
	else
	{
		m_VertexBuffer = readVertexBuffer(m_FileHeader.m_NumVertex, &input);
	}
	m_MaterialBuffer = readMaterialBuffer(m_FileHeader.m_NumMaterialBuffer, &input);
	
}

const std::vector<Material>& ModelBinaryLoader::getMaterial()
{
	return m_Material;
}

const std::vector<AnimatedVertex>& ModelBinaryLoader::getAnimatedVertexBuffer()
{
	return m_AnimationVertexBuffer;
}

const std::vector<StaticVertex>& ModelBinaryLoader::getStaticVertexBuffer()
{
	return m_VertexBuffer;
}

const std::vector<MaterialBuffer>& ModelBinaryLoader::getMaterialBuffer()
{
	return m_MaterialBuffer;
}

void ModelBinaryLoader::clearData()
{
	m_FileHeader.m_ModelName = "";
	m_FileHeader.m_NumMaterial = 0;
	m_FileHeader.m_NumMaterialBuffer = 0;
	m_FileHeader.m_NumVertex = 0;
	m_FileHeader.m_NumJoints = 0;
	m_Material.clear();
	m_AnimationVertexBuffer.clear();
	m_VertexBuffer.clear();
	m_MaterialBuffer.clear();
}