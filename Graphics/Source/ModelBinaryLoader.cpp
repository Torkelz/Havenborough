#include "ModelBinaryLoader.h"

#include "GraphicsExceptions.h"

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
	tempHeader.m_Animated = true;
	tempHeader.m_Transparent = true;
	tempHeader.m_CollideAble = true;
	int tempBool;
	byteToString(p_Input, tempHeader.m_ModelName);
	byteToInt(p_Input, tempHeader.m_NumMaterial);
	byteToInt(p_Input, tempHeader.m_NumVertex);
	byteToInt(p_Input, tempHeader.m_NumMaterialBuffer);
	byteToInt(p_Input, tempBool);
	if(tempBool == 0)
	{
		tempHeader.m_Animated= false;
	}
	byteToInt(p_Input, tempBool);
	if(tempBool == 0)
	{
		tempHeader.m_Transparent = false;
	}
	byteToInt(p_Input, tempBool);
	if(tempBool == 0)
	{
		tempHeader.m_CollideAble = false;
	}
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
		throw GraphicsException("File could not be opened: " + p_FilePath, __LINE__, __FILE__);
	}
	m_FileHeader = readHeader(&input);
	m_Material = readMaterial(m_FileHeader.m_NumMaterial,&input);
	if(m_FileHeader.m_Animated)
	{
		m_AnimationVertexBuffer = readVertexBufferAnimation(m_FileHeader.m_NumVertex, &input);
		calculateBoundingVolume(m_AnimationVertexBuffer);
	}
	else
	{
		m_VertexBuffer = readVertexBuffer(m_FileHeader.m_NumVertex, &input);
		calculateBoundingVolume(m_VertexBuffer);
	}
	m_MaterialBuffer = readMaterialBuffer(m_FileHeader.m_NumMaterialBuffer, &input);
	
}

const std::vector<Material>& ModelBinaryLoader::getMaterial() const
{
	return m_Material;
}

const std::vector<AnimatedVertex>& ModelBinaryLoader::getAnimatedVertexBuffer() const
{
	return m_AnimationVertexBuffer;
}

const std::vector<StaticVertex>& ModelBinaryLoader::getStaticVertexBuffer() const
{
	return m_VertexBuffer;
}

const std::vector<MaterialBuffer>& ModelBinaryLoader::getMaterialBuffer() const
{
	return m_MaterialBuffer;
}

bool ModelBinaryLoader::getAnimated() const
{
	return m_FileHeader.m_Animated;
}

bool ModelBinaryLoader::getTransparent() const
{
	return m_FileHeader.m_Transparent;
}

bool ModelBinaryLoader::getCollideAble() const
{
	return m_FileHeader.m_CollideAble;
}

const std::array<DirectX::XMFLOAT3, 8>& ModelBinaryLoader::getBoundingVolume() const
{
	return m_BoundingVolume;
}

void ModelBinaryLoader::clearData()
{
	m_FileHeader.m_ModelName = "";
	m_FileHeader.m_NumMaterial = 0;
	m_FileHeader.m_NumMaterialBuffer = 0;
	m_FileHeader.m_NumVertex = 0;
	m_FileHeader.m_Animated = false;
	m_FileHeader.m_CollideAble = false;
	m_FileHeader.m_Transparent = false;
	m_Material.clear();
	m_AnimationVertexBuffer.clear();
	m_VertexBuffer.clear();
	m_MaterialBuffer.clear();
}

template <typename VertList>
void calcBoundingVolume(const VertList& p_Vertices, std::array<DirectX::XMFLOAT3, 8>& p_Volume)
{
	DirectX::XMFLOAT3 minPos(
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max());
	DirectX::XMFLOAT3 maxPos(
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min());

	for (const auto& vert : p_Vertices)
	{
		if (vert.m_Position.x < minPos.x) minPos.x = vert.m_Position.x;
		if (vert.m_Position.x > maxPos.x) maxPos.x = vert.m_Position.x;
		if (vert.m_Position.y < minPos.y) minPos.y = vert.m_Position.y;
		if (vert.m_Position.y > maxPos.y) maxPos.y = vert.m_Position.y;
		if (vert.m_Position.z < minPos.z) minPos.z = vert.m_Position.z;
		if (vert.m_Position.z > maxPos.z) maxPos.z = vert.m_Position.z;
	}

	p_Volume[0] = DirectX::XMFLOAT3(minPos.x, minPos.y, minPos.z);
	p_Volume[1] = DirectX::XMFLOAT3(minPos.x, minPos.y, maxPos.z);
	p_Volume[2] = DirectX::XMFLOAT3(minPos.x, maxPos.y, minPos.z);
	p_Volume[3] = DirectX::XMFLOAT3(minPos.x, maxPos.y, maxPos.z);
	p_Volume[4] = DirectX::XMFLOAT3(maxPos.x, minPos.y, minPos.z);
	p_Volume[5] = DirectX::XMFLOAT3(maxPos.x, minPos.y, maxPos.z);
	p_Volume[6] = DirectX::XMFLOAT3(maxPos.x, maxPos.y, minPos.z);
	p_Volume[7] = DirectX::XMFLOAT3(maxPos.x, maxPos.y, maxPos.z);
}

void ModelBinaryLoader::calculateBoundingVolume(const std::vector<AnimatedVertex>& p_Vertices)
{
	calcBoundingVolume(p_Vertices, m_BoundingVolume);
}

void ModelBinaryLoader::calculateBoundingVolume(const std::vector<StaticVertex>& p_Vertices)
{
	calcBoundingVolume(p_Vertices, m_BoundingVolume);
}
