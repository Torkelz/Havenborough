#include "ModelConverter.h"

ModelConverter::ModelConverter()
{
	m_NumberOfFrames = 0;
	m_VertexCount = 0;
	m_End = 0;
	m_Start = 0;
	m_IndexPerMaterial = 0;
	m_MaterialSize = 0;
	m_IndexPerMaterialSize = 0;
	m_ListOfJointsSize = 0;
	m_WeightsListSize = 0;
}

ModelConverter::~ModelConverter()
{
	clear();
}

void ModelConverter::clear()
{
	m_NumberOfFrames = 0;
	m_VertexCount = 0;
	m_End = 0;
	m_Start = 0;
	m_NumberOfFrames = 0;
	m_VertexCount = 0;
	m_End = 0;
	m_Start = 0;
	m_IndexPerMaterial = 0;
	m_MaterialSize = 0;
	m_IndexPerMaterialSize = 0;
	m_ListOfJointsSize = 0;
	m_WeightsListSize = 0;
	m_MeshName = "";
	m_Indices = nullptr;
}

bool ModelConverter::writeFile(std::string p_FilePath)
{
	if (m_MeshName == "")
	{
		return false;
	}
	std::ofstream output(p_FilePath, std::ostream::out | std::ostream::binary);
	if(!output)
	{
		return false;
	}
	createHeader(&output);
	createMaterial(&output);
	if(m_WeightsListSize != 0)
	{
		createVertexBufferAnimation(&output);
		createJointBuffer(&output);
	}
	else
	{
		createVertexBuffer(&output); 
	}
	createMaterialBuffer(&output);
	output.close();
	clearData();
	return true;
}

void ModelConverter::createHeader(std::ostream* p_Output)
{
	//assert(m_IndexPerMaterial != nullptr);
	stringToByte(m_MeshName, p_Output);
	intToByte(m_MaterialSize, p_Output);
	for(int i = 0; i < m_IndexPerMaterialSize; i++)
	{
		m_VertexCount += m_IndexPerMaterial->at(i).size();
	}
	intToByte(m_VertexCount, p_Output);
	intToByte(m_IndexPerMaterialSize, p_Output);
	intToByte(m_ListOfJointsSize, p_Output);
	intToByte(m_NumberOfFrames, p_Output);
}

void ModelConverter::createVertexBuffer(std::ostream* p_Output)
{
	VertexBuffer temp;
	std::vector<VertexBuffer> tempVertex;
	for(int i = 0; i < m_IndexPerMaterialSize; i++)
	{
		int tempsize = m_IndexPerMaterial->at(i).size()-1;
		for(int j = tempsize; j >= 0 ; j--)
		{
			temp.m_Position = DirectX::XMFLOAT4(m_Vertices->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).x,m_Vertices->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).y,m_Vertices->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).z, 1.0f);
			temp.m_Normal = m_Normals->at(m_IndexPerMaterial->at(i).at(j).m_Normal);
			temp.m_UV = m_TextureCoord->at(m_IndexPerMaterial->at(i).at(j).m_TextureCoord);
			temp.m_Tangent = m_Tangents->at(m_IndexPerMaterial->at(i).at(j).m_Tangent);
			DirectX::XMStoreFloat3(&temp.m_Binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&temp.m_Tangent),DirectX::XMLoadFloat3(&temp.m_Normal)));
			//temp.m_Position.x *= -1.f;
			//temp.m_Normal.x *= -1.f;
			//temp.m_Tangent.x *= -1.f;
			//temp.m_Binormal.x *= -1.f;
			tempVertex.push_back(temp);
		}
	}
	int size = sizeof(VertexBuffer) * tempVertex.size();
	p_Output->write(reinterpret_cast<const char*>(tempVertex.data()), size);
 }

void ModelConverter::createVertexBufferAnimation(std::ostream* p_Output)
{
	VertexBufferAnimation temp;
	std::vector<VertexBufferAnimation> tempVertex;
	for(int i = 0; i < m_IndexPerMaterialSize; i++)
	{
		int tempsize = m_IndexPerMaterial->at(i).size()-1;
		for(int j = tempsize; j >= 0 ; j--)
		{
			temp.m_Position = DirectX::XMFLOAT4(m_Vertices->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).x,m_Vertices->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).y,m_Vertices->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).z, 1.0f);
			temp.m_Normal = m_Normals->at(m_IndexPerMaterial->at(i).at(j).m_Normal);
			temp.m_UV = m_TextureCoord->at(m_IndexPerMaterial->at(i).at(j).m_TextureCoord);
			temp.m_Tangent = m_Tangents->at(m_IndexPerMaterial->at(i).at(j).m_Tangent);
			DirectX::XMStoreFloat3(&temp.m_Binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&temp.m_Tangent),DirectX::XMLoadFloat3(&temp.m_Normal)));
			temp.m_Weight = m_WeightsList->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).first;
			temp.m_Joint = m_WeightsList->at(m_IndexPerMaterial->at(i).at(j).m_Vertex).second;
			//temp.m_Position.x *= -1.f;
			//temp.m_Normal.x *= -1.f;
			//temp.m_Tangent.x *= -1.f;
			//temp.m_Binormal.x *= -1.f;
			tempVertex.push_back(temp);
		}
	}
	p_Output->write(reinterpret_cast<const char*>(tempVertex.data()), sizeof(VertexBufferAnimation) * tempVertex.size());
}

void ModelConverter::createMaterialBuffer(std::ostream* p_Output)
{
	int start = 0;
	for(int j = 0; j < m_IndexPerMaterialSize; j++)
	{
		stringToByte(m_IndexPerMaterial->at(j).at(0).m_MaterialID,p_Output);
		intToByte(start, p_Output);
		start += m_IndexPerMaterial->at(j).size();
		intToByte(m_IndexPerMaterial->at(j).size(), p_Output);
	}
}

void ModelConverter::createMaterial(std::ostream* p_Output)
{
	for(int i = 0; i < m_MaterialSize; i++)
	{
		stringToByte(m_Material->at(i).m_MaterialID, p_Output);
		stringToByte(m_Material->at(i).m_DiffuseMap, p_Output);
		stringToByte(m_Material->at(i).m_NormalMap, p_Output);
		stringToByte(m_Material->at(i).m_SpecularMap, p_Output);
	}
}

void ModelConverter::createJointBuffer(std::ostream* p_Output)
{
	for(int i = 0; i < m_ListOfJointsSize; i++)
	{
		stringToByte(m_ListOfJoints->at(i).m_JointName, p_Output);
		intToByte(m_ListOfJoints->at(i).m_ID, p_Output);
		intToByte(m_ListOfJoints->at(i).m_Parent, p_Output);
		p_Output->write(reinterpret_cast<const char*>(&m_ListOfJoints->at(i).m_JointOffsetMatrix), sizeof(DirectX::XMFLOAT4X4));
		p_Output->write(reinterpret_cast<const char*>(m_ListOfJoints->at(i).m_JointAnimation.data()), sizeof(ModelLoader::KeyFrame) * m_NumberOfFrames);
	}
}

void ModelConverter::stringToByte(std::string p_String, std::ostream* p_Output)
{
	unsigned int size = p_String.size();
	p_Output->write(reinterpret_cast<const char*>(&size), sizeof(unsigned int));
	p_Output->write(p_String.data(), p_String.size());
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
	m_IndexPerMaterialSize = m_IndexPerMaterial->size();
}

void ModelConverter::setMaterial(const std::vector<ModelLoader::Material>* p_Material)
{
	m_Material = p_Material;
	m_MaterialSize = m_Material->size();
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

void ModelConverter::setWeightsList(const std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT4>>* p_WeightsList)
{
	m_WeightsList = p_WeightsList;
	m_WeightsListSize = m_WeightsList->size();
}

void ModelConverter::setListOfJoints(const std::vector<ModelLoader::Joint>* p_ListOfJoints)
{
	m_ListOfJoints = p_ListOfJoints;
	m_ListOfJointsSize = m_ListOfJoints->size();
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
	m_NumberOfFrames = 0;
	m_VertexCount = 0;
	m_End = 0;
	m_Start = 0;
	m_IndexPerMaterial = 0;
	m_MaterialSize = 0;
	m_IndexPerMaterialSize = 0;
	m_ListOfJointsSize = 0;
	m_WeightsListSize = 0;
	m_MeshName = "";
}