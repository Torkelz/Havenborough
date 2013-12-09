#include "ModelConverter.h"

ModelConverter::ModelConverter()
{
	m_VertexCount = 0;
}

ModelConverter::~ModelConverter()
{
}

void ModelConverter::clear()
{
	m_Indices.clear();
	m_Indices.shrink_to_fit();
	
	m_NumberOfFrames = 0;
	m_VertexCount = 0;
	m_End = 0;
	m_Start = 0;
	m_MeshName.clear();
	m_MeshName.shrink_to_fit();
}

void ModelConverter::writeFile(std::string p_FilePath)
{
	
	std::ofstream output(p_FilePath, std::ostream::out | std::ostream::binary);
	createHeader(&output);
	createMaterial(&output);
	createVertexBuffer(&output);
	createMaterialBuffer(&output);
	createJointBuffer(&output);
	
	output.close();
	clearData();
}

void ModelConverter::createHeader(std::ostream* p_Output)
{
	stringToByte(m_MeshName, p_Output);
	intToByte(m_Material->size(), p_Output);
	for(unsigned int i = 0; i < m_IndexPerMaterial->size(); i++)
	{
		m_VertexCount += m_IndexPerMaterial->at(i).size();
	}
	intToByte(m_VertexCount, p_Output);
	intToByte(m_IndexPerMaterial->size(), p_Output);
	intToByte(m_ListOfJoints->size(), p_Output);
	intToByte(m_NumberOfFrames, p_Output);
}

void ModelConverter::createVertexBuffer(std::ostream* p_Output)
{
	VertexBuffer temp;
	std::vector<VertexBuffer> tempVertex;
	for(unsigned int i = 0; i < m_IndexPerMaterial->size(); i++)
	{
		for(unsigned int j = 0; j < m_IndexPerMaterial->at(i).size(); j++)
		{
			temp.m_Position = DirectX::XMFLOAT4(m_Vertices->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).x,m_Vertices->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).y,m_Vertices->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).z, 1.0f);
			temp.m_Normal = m_Normals->at(m_IndexPerMaterial->at(i).at(j).m_Normal);
			temp.m_UV = m_TextureCoord->at(m_IndexPerMaterial->at(i).at(j).m_TextureCoord);
			temp.m_Tangent = m_Tangents->at(m_IndexPerMaterial->at(i).at(j).m_Tangent);
			DirectX::XMStoreFloat3(&temp.m_Binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&temp.m_Tangent),DirectX::XMLoadFloat3(&temp.m_Normal)));
			temp.m_Weight = m_WeightsList->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).first;
			temp.m_Joint = m_WeightsList->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).second;
			tempVertex.push_back(temp);
		}
	}
	int size = sizeof(VertexBuffer) * tempVertex.size();
	p_Output->write(reinterpret_cast<const char*>(tempVertex.data()), size);
}

void ModelConverter::createMaterialBuffer(std::ostream* p_Output)
{
	int start = 0;
	int i = 0;
	for(unsigned int j = 0; j < m_IndexPerMaterial->size(); j++)
	{
		stringToByte(m_IndexPerMaterial->at(j).at(i).m_MaterialID,p_Output);
		intToByte(start, p_Output);
		start += m_IndexPerMaterial->at(j).size();
		intToByte(m_IndexPerMaterial->at(j).size(), p_Output);
		i++;
	}
}

void ModelConverter::createMaterial(std::ostream* p_Output)
{
	for(unsigned int i = 0; i < m_Material->size(); i++)
	{
		stringToByte(m_Material->at(i).m_MaterialID, p_Output);
		stringToByte(m_Material->at(i).m_DiffuseMap, p_Output);
		stringToByte(m_Material->at(i).m_NormalMap, p_Output);
		stringToByte(m_Material->at(i).m_SpecularMap, p_Output);
	}
}

void ModelConverter::createJointBuffer(std::ostream* p_Output)
{
	int sizeXMFLOAT4X4 = sizeof(DirectX::XMFLOAT4X4);
	int sizeKEYFRAMES = sizeof(ModelLoader::KeyFrame) * m_NumberOfFrames;
	for(unsigned int i = 0; i < m_ListOfJoints->size(); i++)
	{
		stringToByte(m_ListOfJoints->at(i).m_JointName, p_Output);
		intToByte(m_ListOfJoints->at(i).m_ID, p_Output);
		intToByte(m_ListOfJoints->at(i).m_Parent, p_Output);
		p_Output->write(reinterpret_cast<const char*>(&m_ListOfJoints->at(i).m_JointOffsetMatrix), sizeXMFLOAT4X4);
		p_Output->write(reinterpret_cast<const char*>(m_ListOfJoints->at(i).m_JointAnimation.data()), sizeKEYFRAMES);
	}
}

void ModelConverter::stringToByte(std::string p_String, std::ostream* p_Output)
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

void ModelConverter::intToByte(int p_Int, std::ostream* p_Output)
{
	p_Output->write(reinterpret_cast<const char*>(&p_Int), sizeof(p_Int));
}

void ModelConverter::floatToByte(float p_Float, std::ostream* p_Output)
{
	p_Output->write(reinterpret_cast<const char*>(&p_Float), sizeof(p_Float));
}

void ModelConverter::setVertices(const std::vector<DirectX::XMFLOAT3>* p_Vertices)
{
	m_Vertices = p_Vertices;
}

void ModelConverter::setIndices(const std::vector<std::vector<ModelLoader::IndexDesc>>* p_Indices)
{
	m_IndexPerMaterial = p_Indices;
}

void ModelConverter::setMaterial(const std::vector<ModelLoader::Material>* p_Material)
{
	m_Material = p_Material;
}

void ModelConverter::setNormals(const std::vector<DirectX::XMFLOAT3>* p_Normals)
{
	m_Normals = p_Normals;
}

void ModelConverter::setTangents(const std::vector<DirectX::XMFLOAT3>* p_Tangents)
{
	m_Tangents = p_Tangents;
}

void ModelConverter::setTextureCoords(const std::vector<DirectX::XMFLOAT2>* p_TextureCoord)
{
	m_TextureCoord = p_TextureCoord;
}

void ModelConverter::setWeightsList(const std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT4>>* p_WeightList)
{
	m_WeightsList = p_WeightList;
}

void ModelConverter::setListOfJoints(const std::vector<ModelLoader::Joint>* p_ListOfJoints)
{
	m_ListOfJoints = p_ListOfJoints;
}

void ModelConverter::setAnimationStartValue(float p_StartTime)
{
	m_Start = p_StartTime;
}

void ModelConverter::setAnimationEndValue(float p_EndTime)
{
	m_End = p_EndTime;
}

void ModelConverter::setNumberOfFrames(int p_NumberOfFrames)
{
	m_NumberOfFrames = p_NumberOfFrames;
}

void ModelConverter::setMeshName(std::string p_MeshName)
{
	m_MeshName = p_MeshName;
}

void ModelConverter::clearData()
{
	m_Indices.clear();
	m_NumberOfFrames = 0;
	m_VertexCount = 0;
	m_End = 0;
	m_Start = 0;
	m_MeshName.clear();
}