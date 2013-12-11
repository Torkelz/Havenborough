#include "ModelBinaryLoader.h"


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
	m_Joints.clear();
	m_Joints.shrink_to_fit();
	m_VertexBuffer.clear();
	m_VertexBuffer.shrink_to_fit();
	m_MaterialBuffer.clear();
	m_MaterialBuffer.shrink_to_fit();
}

ModelBinaryLoader::Header ModelBinaryLoader::readHeader(std::istream* p_Input)
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

std::vector<Vertex> ModelBinaryLoader::readVertexBuffer(int p_NumberOfVertex, std::istream* p_Input)
{
	std::vector<Vertex> vertexBuffer(p_NumberOfVertex);
	p_Input->read(reinterpret_cast<char*>(vertexBuffer.data()), sizeof(Vertex) * p_NumberOfVertex);
	return vertexBuffer;
}

std::vector<VertexAnimation> ModelBinaryLoader::readVertexBufferAnimation(int p_NumberOfVertex, std::istream* p_Input)
{
	std::vector<VertexAnimation> vertexBuffer(p_NumberOfVertex);
	p_Input->read(reinterpret_cast<char*>(vertexBuffer.data()), sizeof(VertexAnimation) * p_NumberOfVertex);
	return vertexBuffer;
}

std::vector<Joint> ModelBinaryLoader::readJointList(int p_NumberOfJoint, int p_NumberOfFrames, std::istream* p_Input)
{
	std::vector<Joint> readJoints;
	Joint temp;
	temp.m_JointAnimation.resize(p_NumberOfFrames);
	for(int i = 0; i < p_NumberOfJoint; i++)
	{
		byteToString(p_Input, temp.m_JointName);
		byteToInt(p_Input, temp.m_ID);
		byteToInt(p_Input, temp.m_Parent);
		p_Input->read(reinterpret_cast<char*>(&temp.m_JointOffsetMatrix), sizeof(DirectX::XMFLOAT4X4));
		p_Input->read(reinterpret_cast<char*>(temp.m_JointAnimation.data()), sizeof(KeyFrame) * p_NumberOfFrames);
		readJoints.push_back(temp);
	}
	return readJoints;
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
	p_Input->read((char*)&p_Return, sizeof(unsigned int));
}

void ModelBinaryLoader::byteToFloat(std::istream* p_Input, float& p_Return)
{
	p_Input->read(reinterpret_cast<char*>(&p_Return), sizeof(float) );
}

bool ModelBinaryLoader::loadBinaryFile(std::string p_FilePath)
{
	clearData();
	std::ifstream input(p_FilePath, std::istream::in | std::istream::binary);
	if(!input)
	{
		return false;
	}
	m_FileHeader = readHeader(&input);
	m_Material = readMaterial(m_FileHeader.m_numMaterial,&input);
	if(m_FileHeader.m_numJoints > 0)
	{
		m_AnimationVertexBuffer = readVertexBufferAnimation(m_FileHeader.m_numVertex, &input);
		m_Joints = readJointList(m_FileHeader.m_numJoints, m_FileHeader.m_numFrames, &input);
	}
	else
	{
		m_VertexBuffer = readVertexBuffer(m_FileHeader.m_numVertex, &input);
	}
	m_MaterialBuffer = readMaterialBuffer(m_FileHeader.m_numMaterialBuffer, &input);
	
	return true;
}

const std::vector<Material>& ModelBinaryLoader::getMaterial()
{
	return m_Material;
}

const std::vector<VertexAnimation>& ModelBinaryLoader::getAnimationVertexBuffer()
{
	return m_AnimationVertexBuffer;
}

const std::vector<Joint>& ModelBinaryLoader::getJoints()
{
	return m_Joints;
}

const std::vector<Vertex>& ModelBinaryLoader::getVertexBuffer()
{
	return m_VertexBuffer;
}

const std::vector<MaterialBuffer>& ModelBinaryLoader::getMaterialBuffer()
{
	return m_MaterialBuffer;
}

void ModelBinaryLoader::clearData()
{
	m_FileHeader.m_modelName = "";
	m_FileHeader.m_numFrames = 0;
	m_FileHeader.m_numJoints = 0;
	m_FileHeader.m_numMaterial = 0;
	m_FileHeader.m_numMaterialBuffer = 0;
	m_FileHeader.m_numVertex = 0;
	m_Material.clear();
	m_AnimationVertexBuffer.clear();
	m_Joints.clear();
	m_VertexBuffer.clear();
	m_MaterialBuffer.clear();
}