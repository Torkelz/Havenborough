#pragma warning(disable : 4996)
#include "ModelConverter.h"
#include <fstream>

ModelConverter::ModelConverter()
{
	m_NumberOfFrames = 0;
	m_VertexCount = 0;
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
	m_NumberOfFrames = 0;
	m_VertexCount = 0;
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
	createModelHeaderFile(p_FilePath);
	createMaterial(&output);
	if(m_WeightsListSize != 0)
	{
		createVertexBufferAnimation(&output);

		std::vector<char> outputBuffer(p_FilePath.size()+1);
		strcpy(outputBuffer.data(), p_FilePath.c_str());
		int length = outputBuffer.size();
		strcpy(outputBuffer.data()+length-5, ".atx");
		std::ofstream outputAnimation(outputBuffer.data(), std::ostream::out | std::ostream::binary);
		createAnimationHeader(&outputAnimation);
		createJointBuffer(&outputAnimation);
		outputAnimation.close();
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
	m_Animated = true;
	stringToByte(m_MeshName, p_Output);
	intToByte(m_MaterialSize, p_Output);
	for(int i = 0; i < m_IndexPerMaterialSize; i++)
	{
		m_VertexCount += m_IndexPerMaterial->at(i).size();
	}
	intToByte(m_VertexCount, p_Output);
	intToByte(m_IndexPerMaterialSize, p_Output);
	if(m_ListOfJointsSize == 0)
	{
		m_Animated = false;
	}
	intToByte(m_Animated, p_Output);
	intToByte(m_Transparency, p_Output);
	intToByte(m_Collidable, p_Output);
}

void ModelConverter::createAnimationHeader(std::ostream* p_AnimationOutput)
{
	stringToByte(m_MeshName, p_AnimationOutput);
	intToByte(m_ListOfJointsSize, p_AnimationOutput);
	intToByte(m_NumberOfFrames, p_AnimationOutput);
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
			temp.m_Position.x *= -1.f;
			temp.m_Normal.x *= -1.f;
			temp.m_Tangent.x *= -1.f;
			temp.m_Binormal.x *= -1.f;
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
			const ModelLoader::IndexDesc& desc = m_IndexPerMaterial->at(i).at(j);
			const DirectX::XMFLOAT3& vertexPos = m_Vertices->at(desc.m_Vertex);

			temp.m_Position = DirectX::XMFLOAT4(vertexPos.x,vertexPos.y,vertexPos.z, 1.0f);
			temp.m_Normal = m_Normals->at(desc.m_Normal);
			temp.m_UV = m_TextureCoord->at(desc.m_TextureCoord);
			temp.m_Tangent = m_Tangents->at(desc.m_Tangent);
			DirectX::XMStoreFloat3(&temp.m_Binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&temp.m_Tangent),DirectX::XMLoadFloat3(&temp.m_Normal)));
			temp.m_Weight = m_WeightsList->at(desc.m_Vertex).first;
			temp.m_Joint = m_WeightsList->at(desc.m_Vertex).second;
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
	int size = p_String.size();
	p_Output->write(reinterpret_cast<const char*>(&size), sizeof(int));
	p_Output->write(p_String.data(), p_String.size());
}

void ModelConverter::intToByte(int p_Int, std::ostream* p_Output)
{
	p_Output->write(reinterpret_cast<const char*>(&p_Int), sizeof(p_Int));
}

void ModelConverter::setVertices(const std::vector<DirectX::XMFLOAT3>* p_Vertices)
{
	m_Vertices = p_Vertices;
}

void ModelConverter::setTransparent(bool p_Transparent)
{
	m_Transparency = p_Transparent;
}

void ModelConverter::setCollidable(bool p_Collidable)
{
	m_Collidable = p_Collidable;
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

void ModelConverter::setWeightsList(const std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMINT4>>* p_WeightsList)
{
	m_WeightsList = p_WeightsList;
	m_WeightsListSize = m_WeightsList->size();
}

void ModelConverter::setListOfJoints(const std::vector<ModelLoader::Joint>* p_ListOfJoints)
{
	m_ListOfJoints = p_ListOfJoints;
	m_ListOfJointsSize = m_ListOfJoints->size();
}

void ModelConverter::setNumberOfFrames(int p_NumberOfFrames)
{
	m_NumberOfFrames = p_NumberOfFrames;
}

void ModelConverter::setMeshName(std::string p_MeshName)
{
	m_MeshName = p_MeshName;
}

std::string ModelConverter::getPath(std::string p_FilePath)
{
	std::string file("levels\\ModelHeader.txx");
	std::vector<char> buffer(p_FilePath.begin(), p_FilePath.end());
	buffer.push_back(0);
	char *tmp, *type = nullptr;
	tmp = strtok(buffer.data(), "\\");
	while(tmp != nullptr)
	{
		type = tmp;
		tmp = strtok(NULL,"\\");
	}
	int length = buffer.size();
	int size; 
	if(type == nullptr)
	{
		size = 8;
	}
	else
	{
		size = strlen(type)+8;
	}

	std::string temp;
	if(length < size)
	{
		return "..\\error\\0";
	}
	temp.append(p_FilePath.data(), length-size);
	temp.append(file.data(),file.size());
	temp.push_back(0);
	return temp;
}

bool ModelConverter::createModelHeaderFile(std::string p_FilePath)
{
	std::string path = getPath(p_FilePath);
	std::fstream headerOutput(path, std::fstream::in | std::fstream::out | std::fstream::binary);
	if(!headerOutput)
	{
		headerOutput.open(path, std::fstream::out | std::fstream::binary);
		if(!headerOutput)
		{
			return false;
		}
	}
	std::string meshName;
	std::streampos position;
	int boolValue;
	headerOutput.seekg(0,std::ifstream::end);
	std::streamoff size = headerOutput.tellg();
	headerOutput.seekg(0,std::ifstream::beg);
	while(headerOutput.tellg() < size)
	{
		position = headerOutput.tellg();
		byteToString(headerOutput, meshName);
		if(meshName == m_MeshName)
		{
			headerOutput.seekp(position);
			stringToByte(m_MeshName, &headerOutput);
			intToByte(m_Animated, &headerOutput);
			intToByte(m_Transparency, &headerOutput);
			intToByte(m_Collidable, &headerOutput);
			return true;
		}
		else
		{
			byteToInt(headerOutput, boolValue);
			byteToInt(headerOutput, boolValue);
			byteToInt(headerOutput, boolValue);
		}
	}
	headerOutput.seekp(headerOutput.tellg());
	stringToByte(m_MeshName, &headerOutput);
	intToByte(m_Animated, &headerOutput);
	intToByte(m_Transparency, &headerOutput);
	intToByte(m_Collidable, &headerOutput);
	headerOutput.close();
	return true;
}

void ModelConverter::byteToString(std::istream& p_Input, std::string& p_Return)
{
	int strLength = 0;
	byteToInt(p_Input, strLength);
	std::vector<char> buffer(strLength);
	p_Input.read( buffer.data(), strLength);
	p_Return = std::string(buffer.data(), strLength);
}

void ModelConverter::byteToInt(std::istream& p_Input, int& p_Return)
{
	p_Input.read((char*)&p_Return, sizeof(int));
}

void ModelConverter::clearData()
{
	m_NumberOfFrames = 0;
	m_VertexCount = 0;
	m_IndexPerMaterial = 0;
	m_MaterialSize = 0;
	m_IndexPerMaterialSize = 0;
	m_ListOfJointsSize = 0;
	m_WeightsListSize = 0;
	m_MeshName = "";
}