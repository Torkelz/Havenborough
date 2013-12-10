#include "ModelBinaryLoader.h"


ModelBinaryLoader::ModelBinaryLoader()
{

}

ModelBinaryLoader::~ModelBinaryLoader()
{

}

ModelBinaryLoader::Header ModelBinaryLoader::readHeader(std::istream* p_Input)
{
	Header tempHeader;
	byteToString(p_Input, &tempHeader.m_modelName);
	byteToInt(p_Input, &tempHeader.m_numMaterial);
	byteToInt(p_Input, &tempHeader.m_numVertex);
	byteToInt(p_Input, &tempHeader.m_numMaterialBuffer);
	byteToInt(p_Input, &tempHeader.m_numJoints);
	byteToInt(p_Input, &tempHeader.m_numFrames);
	return tempHeader;
}

std::vector<ModelBinaryLoader::Material> ModelBinaryLoader::readMaterial(int p_NumberOfMaterial, std::istream* p_Input)
{
	ModelBinaryLoader::Material temp;
	std::vector<ModelBinaryLoader::Material> tempVector;
	for(int i = 0; i < p_NumberOfMaterial; i++)
	{
		byteToString(p_Input, &temp.m_MaterialID);
		byteToString(p_Input, &temp.m_DiffuseMap);
		byteToString(p_Input, &temp.m_NormalMap);
		byteToString(p_Input, &temp.m_SpecularMap);
		tempVector.push_back(temp);
	}
	return tempVector;
}

std::vector<ModelBinaryLoader::MaterialBuffer> ModelBinaryLoader::readMaterialBuffer(int p_NumberOfMaterialBuffers, std::istream* p_Input)
{
	MaterialBuffer temp;
	std::vector<MaterialBuffer> tempBuffer;
	for(int i = 0; i < p_NumberOfMaterialBuffers; i++)
	{
		byteToString(p_Input, &temp.material);
		byteToInt(p_Input, &temp.start);
		byteToInt(p_Input, &temp.length);
		tempBuffer.push_back(temp);
	}
	return tempBuffer;
}

std::vector<ModelBinaryLoader::VertexBuffer> ModelBinaryLoader::readVertexBuffer(int p_NumberOfVertex, std::istream* p_Input)
{
	std::vector<VertexBuffer> vertexBuffer(p_NumberOfVertex);
	int size = sizeof(VertexBuffer) * p_NumberOfVertex;
	p_Input->read(reinterpret_cast<char*>(vertexBuffer.data()), size);
	return vertexBuffer;
}

std::vector<ModelBinaryLoader::Joint> ModelBinaryLoader::readJointList(int p_NumberOfJoint, int p_NumberOfFrames, std::istream* p_Input)
{
	std::vector<Joint> readJoints;
	Joint temp;
	temp.m_JointAnimation.resize(p_NumberOfFrames);
	int sizeMATRIX = sizeof(DirectX::XMFLOAT4X4);
	int sizeKEYFRAMES = sizeof(KeyFrame) * p_NumberOfFrames;
	for(int i = 0; i < p_NumberOfJoint; i++)
	{
		byteToString(p_Input, &temp.m_JointName);
		byteToInt(p_Input, &temp.m_ID);
		byteToInt(p_Input, &temp.m_Parent);
		p_Input->read(reinterpret_cast<char*>(&temp.m_JointOffsetMatrix), sizeMATRIX);
		p_Input->read(reinterpret_cast<char*>(temp.m_JointAnimation.data()), sizeKEYFRAMES);
		readJoints.push_back(temp);
	}
	return readJoints;
}

void ModelBinaryLoader::byteToString(std::istream* p_Input, std::string* p_Return)
{
	int strLength;
	byteToInt(p_Input, &strLength);
	int pos = (int)p_Input->tellg();
    p_Input->seekg( pos + 1 );
	char* c = new char[strLength];
	p_Input->read( c, strLength);
	*p_Return = std::string(c, strLength);
}

void ModelBinaryLoader::byteToInt(std::istream* p_Input, int* p_Return)
{
	char b[4];
	p_Input->read(b, sizeof(unsigned int));
	memcpy(p_Return, b, sizeof(unsigned int));
}

void ModelBinaryLoader::byteToFloat(std::istream* p_Input, float* p_Return)
{
	char b[4];
	p_Input->read( b, sizeof(float) );
	memcpy( p_Return, b, sizeof(float) );
}

void ModelBinaryLoader::byteToFloat4(std::istream* p_Input, DirectX::XMFLOAT4* p_Return)
{
	float tempValue;
	byteToFloat(p_Input, &tempValue);
	p_Return->x = tempValue;
	byteToFloat(p_Input, &tempValue);
	p_Return->y = tempValue;
	byteToFloat(p_Input, &tempValue);
	p_Return->z = tempValue;
	byteToFloat(p_Input, &tempValue);
	p_Return->w = tempValue;
}

void ModelBinaryLoader::byteToFloat3(std::istream* p_Input, DirectX::XMFLOAT3* p_Return)
{
	float tempValue;
	byteToFloat(p_Input, &tempValue);
	p_Return->x = tempValue;
	byteToFloat(p_Input, &tempValue);
	p_Return->y = tempValue;
	byteToFloat(p_Input, &tempValue);
	p_Return->z = tempValue;
}

void ModelBinaryLoader::byteToFloat2(std::istream* p_Input, DirectX::XMFLOAT2* p_Return)
{
	float tempValue;
	byteToFloat(p_Input, &tempValue);
	p_Return->x = tempValue;
	byteToFloat(p_Input, &tempValue);
	p_Return->y = tempValue;
}

bool ModelBinaryLoader::loadBinaryFile(std::string p_FilePath)
{
	std::ifstream input(p_FilePath, std::istream::in | std::istream::binary);
	Header fileHeader = readHeader(&input);
	std::vector<Material> temp = readMaterial(fileHeader.m_numMaterial,&input);
	std::vector<VertexBuffer> test = readVertexBuffer(fileHeader.m_numVertex, &input);
	std::vector<MaterialBuffer> testBuf = readMaterialBuffer(fileHeader.m_numMaterialBuffer, &input);
	std::vector<Joint> testJoint = readJointList(fileHeader.m_numJoints, fileHeader.m_numFrames, &input);
	return true;
}