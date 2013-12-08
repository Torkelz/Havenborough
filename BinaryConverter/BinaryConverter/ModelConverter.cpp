#include "ModelConverter.h"

#define WRITE_SIZE 4096

ModelConverter::ModelConverter()
{
	m_VertexCount = 0;
}

ModelConverter::~ModelConverter()
{
	m_Indices.clear();
	m_Normals.clear();
	m_Vertices.clear();
	m_Tangents.clear();
	m_TextureCoord.clear();	
}

void ModelConverter::clear()
{
	m_Indices.clear();
	m_Indices.shrink_to_fit();
	m_Normals.clear();
	m_Normals.shrink_to_fit();
	m_Tangents.clear();
	m_Tangents.shrink_to_fit();
	m_TextureCoord.clear();
	m_TextureCoord.shrink_to_fit();
	m_Vertices.clear();
	m_Vertices.shrink_to_fit();
	m_IndexPerMaterial.clear();
	m_IndexPerMaterial.shrink_to_fit();
	m_Material.clear();
	m_Material.shrink_to_fit();
	m_WeightsList.clear();
	m_WeightsList.shrink_to_fit();
	m_ListOfJoints.clear();
	m_ListOfJoints.shrink_to_fit();

	m_End = 0;
	m_Start = 0;
	m_NumberOfMaterials = 0;
	m_MeshName.clear();
	m_MeshName.shrink_to_fit();
	m_NumberOfVertices = 0;
	m_NumberOfTriangles = 0;
}

void ModelConverter::writeFile(std::string p_FilePath)
{
	std::ofstream output(p_FilePath, std::ofstream::out | std::ofstream::binary);
	createHeader(&output);
	createMaterial(&output);
	createVertexBuffer(&output);
	createMaterialBuffer(&output);
	
	output.close();
}

void ModelConverter::createHeader(std::ofstream* p_Output)
{
	Header header;
	stringToByte(m_MeshName, p_Output);
	intToByte(m_Material.size(), p_Output);
	for(unsigned int i = 0; i < m_IndexPerMaterial.size(); i++)
	{
		m_VertexCount += m_IndexPerMaterial.at(i).size();
	}
	intToByte(m_VertexCount, p_Output);
	intToByte(m_IndexPerMaterial.size(), p_Output);
}

void ModelConverter::createVertexBuffer(std::ofstream* p_Output)
{
	DirectX::XMFLOAT3 binormal, tangent, normal, vertex;
	DirectX::XMFLOAT2 uv;
	for(unsigned int i = 0; i < m_IndexPerMaterial.size(); i++)
	{
		for(unsigned int j = 0; j < m_IndexPerMaterial.at(i).size(); j++)
		{
			floatToByte(1.0f, p_Output);
			vertex = m_Vertices.at(m_IndexPerMaterial.at(i).at(j).m_Vertex);
			floatToByte(vertex.z, p_Output);
			floatToByte(vertex.y, p_Output);
			floatToByte(vertex.x, p_Output);
			normal = m_Normals.at(m_IndexPerMaterial.at(i).at(j).m_Normal);
			floatToByte(normal.z, p_Output);
			floatToByte(normal.y, p_Output);
			floatToByte(normal.x, p_Output);
			uv = m_TextureCoord.at(m_IndexPerMaterial.at(i).at(j).m_TextureCoord);
			floatToByte(uv.y, p_Output);
			floatToByte(uv.x, p_Output);
			tangent = m_Tangents.at(m_IndexPerMaterial.at(i).at(j).m_Tangent);
			floatToByte(tangent.z, p_Output);
			floatToByte(tangent.y, p_Output);
			floatToByte(tangent.x, p_Output);
			DirectX::XMStoreFloat3(&binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&tangent),DirectX::XMLoadFloat3(&normal)));
			floatToByte(binormal.z, p_Output);
			floatToByte(binormal.y, p_Output);
			floatToByte(binormal.x, p_Output);
		}
	}
}

void ModelConverter::createMaterialBuffer(std::ofstream* p_Output)
{
	int start = 0;
	int i = 0;
	for(auto &t : m_IndexPerMaterial)
	{
		stringToByte(t.at(i).m_MaterialID,p_Output);
		intToByte(start, p_Output);
		start += t.size();
		intToByte(t.size(), p_Output);
		i++;
	}
}

void ModelConverter::createMaterial(std::ofstream* p_Output)
{
	for(auto &t : m_Material)
	{
		stringToByte(t.m_MaterialID, p_Output);
		stringToByte(t.m_DiffuseMap, p_Output);
		stringToByte(t.m_NormalMap, p_Output);
		stringToByte(t.m_SpecularMap, p_Output);
	}
}
ModelConverter::Header ModelConverter::readHeader(std::ifstream* p_Input)
{
	Header tempHeader;
	tempHeader.m_modelName = byteToString(byteToInt(p_Input), p_Input);
	tempHeader.m_numMaterial = byteToInt(p_Input);
	tempHeader.m_numVertex = byteToInt(p_Input);
	tempHeader.m_numMaterialBuffer = byteToInt(p_Input);
	return tempHeader;
}

std::vector<ModelConverter::Material> ModelConverter::readMaterial(int p_NumberOfMaterial, std::ifstream* p_Input)
{
	Material temp;
	std::vector<Material> tempVector;
	for(int i = 0; i < p_NumberOfMaterial; i++)
	{
		temp.m_MaterialID = byteToString(byteToInt(p_Input), p_Input);
		temp.m_DiffuseMap = byteToString(byteToInt(p_Input), p_Input);
		temp.m_NormalMap = byteToString(byteToInt(p_Input), p_Input);
		temp.m_SpecularMap = byteToString(byteToInt(p_Input), p_Input);
		tempVector.push_back(temp);
	}
	return tempVector;
}

std::vector<ModelConverter::MaterialBuffer> ModelConverter::readMaterialBuffer(int p_NumberOfMaterialBuffers, std::ifstream* p_Input)
{
	MaterialBuffer temp;
	std::vector<MaterialBuffer> tempBuffer;
	for(int i = 0; i < p_NumberOfMaterialBuffers; i++)
	{
		temp.material = byteToString(byteToInt(p_Input),p_Input);
		temp.start = byteToInt(p_Input);
		temp.length = byteToInt(p_Input);
		tempBuffer.push_back(temp);
	}
	return tempBuffer;
}

std::vector<ModelConverter::VertexBuffer> ModelConverter::readVertexBuffer(int p_NumberOfVertex, std::ifstream* p_Input)
{
	std::vector<VertexBuffer> vertexBuffer; 
	VertexBuffer tempVertex;

	for(int i = 0; i < p_NumberOfVertex; i++)
	{
		tempVertex.m_Position = DirectX::XMFLOAT4(byteToFloat(p_Input),byteToFloat(p_Input),byteToFloat(p_Input),byteToFloat(p_Input));
		tempVertex.m_Normal = DirectX::XMFLOAT3(byteToFloat(p_Input),byteToFloat(p_Input),byteToFloat(p_Input));
		tempVertex.m_UV = DirectX::XMFLOAT2(byteToFloat(p_Input),byteToFloat(p_Input));
		tempVertex.m_Tangent = DirectX::XMFLOAT3(byteToFloat(p_Input),byteToFloat(p_Input),byteToFloat(p_Input));
		tempVertex.m_Binormal = DirectX::XMFLOAT3(byteToFloat(p_Input),byteToFloat(p_Input),byteToFloat(p_Input));
		vertexBuffer.push_back(tempVertex);
	}
	return vertexBuffer;
}

void ModelConverter::stringToByte(std::string p_String, std::ofstream* p_Output)
{
	unsigned int size = p_String.size();
	char* data = new char[size];
	p_Output->write(reinterpret_cast<const char*>(&size), sizeof(size));
	for(unsigned int i = 0; i < size; i++)
	{
		data[1+i] = p_String[i];
	}
	size++;
	p_Output->write(data, size);
}

void ModelConverter::intToByte(int p_Int, std::ofstream* p_Output)
{
	p_Output->write(reinterpret_cast<const char*>(&p_Int), sizeof(p_Int));
}

void ModelConverter::floatToByte(float p_Float, std::ofstream* p_Output)
{
	p_Output->write(reinterpret_cast<const char*>(&p_Float), sizeof(p_Float));
}

std::string ModelConverter::byteToString(int strLength, std::ifstream* p_Input)
{
	int pos = (int)p_Input->tellg();
    p_Input->seekg( pos + 1 );
	char* c = new char[strLength];
	p_Input->read( c, strLength);
	std::string s(c, strLength);
	return s;
}

int ModelConverter::byteToInt(std::ifstream* p_Input)
{
	char b[4];
	int temp = 0;
	p_Input->read(b, sizeof(unsigned int));
	memcpy(&temp, b, sizeof(unsigned int));
	return temp;
}

float ModelConverter::byteToFloat(std::ifstream* p_Input)
{
	char b[4];
	float temp = 0;;

	p_Input->read( b, sizeof(float) );
	memcpy( &temp, b, sizeof(float) );

	return temp;
}

bool ModelConverter::loadBinaryFile(std::string p_FilePath)
{
	std::ifstream input(p_FilePath, std::ifstream::in | std::ifstream::binary);
	Header fileHeader = readHeader(&input);
	std::vector<Material> temp = readMaterial(fileHeader.m_numMaterial,&input);
	std::vector<VertexBuffer> test = readVertexBuffer(fileHeader.m_numVertex, &input);
	std::vector<MaterialBuffer> testBuf = readMaterialBuffer(fileHeader.m_numMaterialBuffer, &input);
	return true;
}

bool ModelConverter::loadFile(std::string p_FilePath)
{
	clearData();
	std::ifstream input(p_FilePath.c_str(),std::ifstream::in);
	std::string line, key, filler;
	int readMaterial = 0;
	int tempInt;
	Material tempMaterial;
	IndexDesc tempFace;
	DirectX::XMFLOAT3 tempFloat3;
	DirectX::XMFLOAT4 tempWeight, tempJoint;
	std::stringstream stringstream;
	Joint tempJointStruct;

	if(!input)
	{
		return false;
	}

	while (!input.eof() && std::getline(input, line))
	{

		key = "";
		stringstream = std::stringstream(line);
		stringstream >> key >> std::ws;
		if(strcmp(key.c_str(), "*Header") == 0)
		{
			std::getline(input, line);
			stringstream = std::stringstream(line);
			stringstream >> key >> m_NumberOfMaterials;
			std::getline(input, line);
			stringstream = std::stringstream(line);
			stringstream >> key >> m_MeshName;
			std::getline(input, line);
			stringstream = std::stringstream(line);
			stringstream >> key >> m_NumberOfVertices;
			std::getline(input, line);
			stringstream = std::stringstream(line);
			stringstream >> key >> m_NumberOfTriangles;
		}
		else if(key  == "*Materials")
		{
			std::getline(input, line);
			stringstream = std::stringstream(line);
			for(int i = 0; i < m_NumberOfMaterials; i++)
			{
				stringstream >> key >> tempMaterial.m_MaterialID;
				std::getline(input, line);
				stringstream = std::stringstream(line);
				stringstream >> key >> tempMaterial.m_DiffuseMap;
				std::getline(input, line);
				stringstream = std::stringstream(line);
				stringstream >> key >> tempMaterial.m_NormalMap;
				std::getline(input, line);
				stringstream = std::stringstream(line);
				stringstream >> key >> tempMaterial.m_SpecularMap;
				std::getline(input, line);
				stringstream = std::stringstream(line);
				if(i != m_NumberOfMaterials-1)
				{
					std::getline(input, line);
					stringstream = std::stringstream(line);
				}
				m_Material.push_back(tempMaterial);
			}
		}
		else if(key == "*Vertices")
		{
			for(int i = 0; i < m_NumberOfVertices; i++)
			{	
				std::getline(input, line);
				stringstream = std::stringstream(line);
				stringstream >> key >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z;
				m_Vertices.push_back(tempFloat3);
			}
		}
		else if(key == "*Normals")
		{
			while(std::getline(input, line))
			{
				stringstream = std::stringstream(line);
				if(line == "")
					break;
				stringstream >> key >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z;
				m_Normals.push_back(tempFloat3);
			}
		}
		else if(key == "*UV")
		{
			while(std::getline(input, line))
			{
				stringstream = std::stringstream(line);
				if(line == "")
					break;
				stringstream >> key >> tempFloat3.x >> tempFloat3.y;
				m_TextureCoord.push_back(DirectX::XMFLOAT2(tempFloat3.x, 1 - tempFloat3.y));
			}
		}
		else if(key == "*Tangets")
		{
			while(std::getline(input, line))
			{
				stringstream = std::stringstream(line);
				if(line == "")
					break;
				stringstream >> key >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z;
				m_Tangents.push_back(tempFloat3);
			}
		}
		else if(key == "*FACES")
		{
			for(int i = 0; i < m_NumberOfMaterials; i++)
			{
				m_Indices.clear();
				std::getline(input, line);
				stringstream = std::stringstream(line);
				stringstream >> tempFace.m_MaterialID;
				std::getline(input, line);
				stringstream = std::stringstream(line);
				stringstream >> filler >> key;
				while(std::getline(input,line))
				{
					stringstream = std::stringstream(line);
					if(line == "")
						break;
					for(int i = 0; i < atoi(key.c_str()); i++)
					{
						stringstream >> tempInt >> filler;
						tempFace.m_Vertex = tempInt;
						stringstream >> tempInt >> filler;
						tempFace.m_Tangent = tempInt;
						stringstream >> tempInt >> filler;
						tempFace.m_Normal = tempInt;
						stringstream >> tempInt >> filler;	
						tempFace.m_TextureCoord = tempInt;
						m_Indices.push_back(tempFace);
					}
					
				}
				m_IndexPerMaterial.push_back(m_Indices);
			}
		}
		else if(key == "*WEIGHTS")
		{
			while(std::getline(input, line))
			{
				stringstream = std::stringstream(line);
				if(line == "")
					break;
				stringstream >> filler >> tempWeight.x >> tempWeight.y >> tempWeight.z >> tempWeight.w;
				std::getline(input, line);
				stringstream = std::stringstream(line);
				stringstream >> filler >> tempJoint.x >> tempJoint.y >> tempJoint.z >> tempJoint.w;
				m_WeightsList.push_back(std::make_pair(tempWeight, tempJoint));
			}
		}
		else if(key == "*HIERARCHY")
		{
			
			while(std::getline(input, line))
			{
				stringstream = std::stringstream(line);
				if(line == "")
					break;
				stringstream >> tempJointStruct.m_JointName >> tempJointStruct.m_ID >> filler >> tempJointStruct.m_Parent;
				m_ListOfJoints.push_back(tempJointStruct);
			}
		}
		else if(key == "*JOINT_OFFSET")
		{
			DirectX::XMFLOAT4X4 tempMat4x4;
			for(unsigned int i = 0; i < m_ListOfJoints.size(); i++)
			{
				std::getline(input, line);
				stringstream = std::stringstream(line);
				if(line == "")
					break;
				stringstream >> filler 
					>> tempMat4x4._11 >> tempMat4x4._12 >> tempMat4x4._13 >> tempMat4x4._14 
					>> tempMat4x4._21 >> tempMat4x4._22	>> tempMat4x4._23 >> tempMat4x4._24
					>> tempMat4x4._31 >> tempMat4x4._32 >> tempMat4x4._33 >> tempMat4x4._34
					>> tempMat4x4._41 >> tempMat4x4._42 >> tempMat4x4._43 >> tempMat4x4._44;
				m_ListOfJoints.at(i).m_JointOffsetMatrix = tempMat4x4;
			}
		}
		else if(key == "*ANIMATION")
		{
			KeyFrame tempKeyFrame;
			std::getline(input, line);
			stringstream = std::stringstream(line);
			stringstream >> filler >> m_Start;
			std::getline(input, line);
			stringstream = std::stringstream(line);
			stringstream >> filler >> m_End;
			std::getline(input, line);
			stringstream = std::stringstream(line);
			stringstream >> filler >> m_NumberOfFrames;
			std::getline(input, line);
			for(unsigned int i = 0; i < m_ListOfJoints.size(); i++)
			{
				std::getline(input, line);
				for(int j = 0; j < m_NumberOfFrames; j++)
				{
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> filler 
						>> tempKeyFrame.m_Trans.x >> tempKeyFrame.m_Trans.y >> tempKeyFrame.m_Trans.z >> filler
						>> tempKeyFrame.m_Rot.x >> tempKeyFrame.m_Rot.y >> tempKeyFrame.m_Rot.z >> tempKeyFrame.m_Rot.w >> filler
						>> tempKeyFrame.m_Scale.x >> tempKeyFrame.m_Scale.y >> tempKeyFrame.m_Scale.z;
					m_ListOfJoints.at(i).m_JointAnimation.push_back(tempKeyFrame);
				}
			}
		}
	}
	return true;
}

const std::vector<DirectX::XMFLOAT3>& ModelConverter::getVertices()
{
	return m_Vertices;
}

const std::vector<std::vector<ModelConverter::IndexDesc>>& ModelConverter::getIndices()
{
	return m_IndexPerMaterial;
}

const std::vector<ModelConverter::Material>& ModelConverter::getMaterial()
{
	return m_Material;
}

const std::vector<DirectX::XMFLOAT3>& ModelConverter::getNormals()
{
	return m_Normals;
}

const std::vector<DirectX::XMFLOAT3>& ModelConverter::getTangents()
{
	return m_Tangents;
}

const std::vector<DirectX::XMFLOAT2>& ModelConverter::getTextureCoords()
{
	return m_TextureCoord;
}

int ModelConverter::getNumberOfMaterial()
{
	return m_NumberOfMaterials;
}

int ModelConverter::getNumberOfVertices()
{
	return m_NumberOfVertices;
}

const std::vector<std::pair<DirectX::XMFLOAT4, DirectX::XMFLOAT4>>& ModelConverter::getWeightsList()
{
	return m_WeightsList;
}

const std::vector<ModelConverter::Joint>& ModelConverter::getListOfJoints()
{
	return m_ListOfJoints;
}

float ModelConverter::getAnimationStartValue()
{
	return m_Start;
}

float ModelConverter::getAnimationEndValue()
{
	return m_End;
}

int ModelConverter::getNumberOfFrames()
{
	return m_NumberOfFrames;
}

void ModelConverter::clearData()
{
	m_Indices.clear();
	m_Normals.clear();
	m_Tangents.clear();
	m_TextureCoord.clear();
	m_Vertices.clear();
	m_IndexPerMaterial.clear();
	m_Material.clear();
	m_WeightsList.clear();
	m_ListOfJoints.clear();

	m_End = 0;
	m_Start = 0;
	m_NumberOfMaterials = 0;
	m_MeshName.clear();
	m_NumberOfVertices = 0;
	m_NumberOfTriangles = 0;
}
