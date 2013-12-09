#include "ModelLoader.h"

ModelLoader::ModelLoader()
{
}

ModelLoader::~ModelLoader()
{
	m_Indices.clear();
	m_Normals.clear();
	m_Vertices.clear();
	m_Tangents.clear();
	m_TextureCoord.clear();	
}

void ModelLoader::clear()
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

bool ModelLoader::loadFile(std::string p_FilePath)
{
	clearData();
	std::ifstream input(p_FilePath.c_str(),std::ifstream::in);
	std::string line, key, filler;
	int readMaterial = 0;
	int tempInt;
	Material tempMaterial;
	IndexDesc tempFace;
	DirectX::XMFLOAT3 tempFloat3;
	DirectX::XMFLOAT3 tempWeight;
	DirectX::XMFLOAT4 tempJoint;
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
				stringstream >> filler >> tempWeight.x >> tempWeight.y >> tempWeight.z;
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

const std::vector<DirectX::XMFLOAT3>& ModelLoader::getVertices()
{
	return m_Vertices;
}

const std::vector<std::vector<ModelLoader::IndexDesc>>& ModelLoader::getIndices()
{
	return m_IndexPerMaterial;
}

const std::vector<ModelLoader::Material>& ModelLoader::getMaterial()
{
	return m_Material;
}

const std::vector<DirectX::XMFLOAT3>& ModelLoader::getNormals()
{
	return m_Normals;
}

const std::vector<DirectX::XMFLOAT3>& ModelLoader::getTangents()
{
	return m_Tangents;
}

const std::vector<DirectX::XMFLOAT2>& ModelLoader::getTextureCoords()
{
	return m_TextureCoord;
}

int ModelLoader::getNumberOfMaterial()
{
	return m_NumberOfMaterials;
}

int ModelLoader::getNumberOfVertices()
{
	return m_NumberOfVertices;
}

const std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT4>>& ModelLoader::getWeightsList()
{
	return m_WeightsList;
}

const std::vector<ModelLoader::Joint>& ModelLoader::getListOfJoints()
{
	return m_ListOfJoints;
}

float ModelLoader::getAnimationStartValue()
{
	return m_Start;
}

float ModelLoader::getAnimationEndValue()
{
	return m_End;
}

int ModelLoader::getNumberOfFrames()
{
	return m_NumberOfFrames;
}

void ModelLoader::clearData()
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