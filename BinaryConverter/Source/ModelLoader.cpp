#include "ModelLoader.h"
#include <iostream>

ModelLoader::ModelLoader()
{
	m_NumberOfFrames = m_NumberOfVertices = m_NumberOfTriangles = m_NumberOfMaterials = 0;
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
	m_NumberOfFrames = 0;
}

bool ModelLoader::loadFile(std::string p_FilePath, std::string p_ResourceListLocation)
{
	clearData();
	std::ifstream input(p_FilePath, std::ifstream::in);
	if(!input)
	{
		return false;
	}
	startReading(input);
	printOutResourceInfo(p_ResourceListLocation);

	input.close();

	return true;
}	
void ModelLoader::startReading(std::istream& p_Input)	
{
	std::string line, key;
	std::stringstream stringstream;
	while (!p_Input.eof() && std::getline(p_Input, line))
	{

		key = "";
		stringstream = std::stringstream(line);
		stringstream >> key >> std::ws;
		if(strcmp(key.c_str(), "*Header") == 0)
		{
			readHeader(p_Input);
		}
		else if(key  == "*Materials")
		{
			readMaterials(p_Input);
		}
		else if(key == "*Vertices")
		{
			readVertex(p_Input);
		}
		else if(key == "*Normals")
		{
			readNormals(p_Input);
		}
		else if(key == "*UV")
		{
			readUV(p_Input);
		}
		else if(key == "*Tangets")
		{
			readTangents(p_Input);
		}
		else if(key == "*FACES")
		{
			readFaces(p_Input);
		}
		else if(key == "*WEIGHTS")
		{
			readWeights(p_Input);
		}
		else if(key == "*HIERARCHY")
		{
			readHierarchy(p_Input);
		}
		else if(key == "*JOINT_OFFSET")
		{
			readJointOffset(p_Input);
		}
		else if(key == "*ANIMATION")
		{
			readAnimation(p_Input);
		}
	}
}

void ModelLoader::readHeader(std::istream& p_Input)
{
	std::string line, key;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> m_Transparent;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> m_Collidable;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> m_NumberOfMaterials;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> m_MeshName;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> m_NumberOfVertices;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> key >> m_NumberOfTriangles;
}

void ModelLoader::readMaterials(std::istream& p_Input)
{
	std::string line, key;
	Material tempMaterial;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	for(int i = 0; i < m_NumberOfMaterials; i++)
	{
		m_Stringstream >> key >> tempMaterial.m_MaterialID;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> key >> tempMaterial.m_DiffuseMap;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> key >> tempMaterial.m_NormalMap;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> key >> tempMaterial.m_SpecularMap;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		if(i != m_NumberOfMaterials-1)
		{
			std::getline(p_Input, line);
			m_Stringstream = std::stringstream(line);
		}
		m_Material.push_back(tempMaterial);
	}
}

void ModelLoader::readVertex(std::istream& p_Input)
{
	DirectX::XMFLOAT3 tempFloat3;
	std::string line, key;
	while(std::getline(p_Input, line))
	{	
		m_Stringstream = std::stringstream(line);
		if(line == "")
			break;
		m_Stringstream >> key >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z;
		m_Vertices.push_back(tempFloat3);
	}
}

void ModelLoader::readNormals(std::istream& p_Input)
{
	DirectX::XMFLOAT3 tempFloat3;
	std::string line, key;
	while(std::getline(p_Input, line))
	{
		m_Stringstream = std::stringstream(line);
		if(line == "")
			break;
		m_Stringstream >> key >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z;
		m_Normals.push_back(tempFloat3);
	}
}

void ModelLoader::readUV(std::istream& p_Input)
{
	DirectX::XMFLOAT3 tempFloat3;
	std::string line, key;
	while(std::getline(p_Input, line))
	{
		m_Stringstream = std::stringstream(line);
		if(line == "")
			break;
		m_Stringstream >> key >> tempFloat3.x >> tempFloat3.y;
		m_TextureCoord.push_back(DirectX::XMFLOAT2(tempFloat3.x, 1 - tempFloat3.y));
	}
}

void ModelLoader::readTangents(std::istream& p_Input)
{
	DirectX::XMFLOAT3 tempFloat3;
	std::string line, key;
	while(std::getline(p_Input, line))
	{
		m_Stringstream = std::stringstream(line);
		if(line == "")
			break;
		m_Stringstream >> key >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z;
		m_Tangents.push_back(tempFloat3);
	}
}

void ModelLoader::readFaces(std::istream& p_Input)
{
	int tempInt;
	IndexDesc tempFace;
	std::string line, key, filler;
	if(m_NumberOfMaterials != 0)
	{
		for(int i = 0; i < m_NumberOfMaterials; i++)
		{
			m_Indices.clear();
			std::getline(p_Input, line);
			m_Stringstream = std::stringstream(line);
			m_Stringstream >> tempFace.m_MaterialID;
			std::getline(p_Input, line);
			m_Stringstream = std::stringstream(line);
			m_Stringstream >> filler >> key;
			while(std::getline(p_Input,line))
			{
				m_Stringstream = std::stringstream(line);
				if(line == "")
					break;
				for(int i = 0; i < atoi(key.c_str()); i++)
				{
					m_Stringstream >> tempInt >> filler;
					tempFace.m_Vertex = tempInt;
					m_Stringstream >> tempInt >> filler;
					tempFace.m_Tangent = tempInt;
					m_Stringstream >> tempInt >> filler;
					tempFace.m_Normal = tempInt;
					m_Stringstream >> tempInt >> filler;	
					tempFace.m_TextureCoord = tempInt;
					m_Indices.push_back(tempFace);
				}
					
			}
			m_IndexPerMaterial.push_back(m_Indices);
		}
	}
	else
	{
		m_Indices.clear();
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> tempFace.m_MaterialID;
		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> key;
		while(std::getline(p_Input,line))
		{
			m_Stringstream = std::stringstream(line);
			if(line == "")
				break;
			for(int i = 0; i < atoi(key.c_str()); i++)
			{
				m_Stringstream >> tempInt >> filler;
				tempFace.m_Vertex = tempInt;
				tempFace.m_Tangent = 0;
				tempFace.m_Normal = 0;	
				tempFace.m_TextureCoord = 0;
				m_Indices.push_back(tempFace);
			}
					
		}
		m_IndexPerMaterial.push_back(m_Indices);
	}

}

void ModelLoader::readWeights(std::istream& p_Input)
{
	DirectX::XMFLOAT3 tempWeight;
	DirectX::XMINT4 tempJoint;
	std::string line, key, filler;

	while(std::getline(p_Input, line))
	{
		m_Stringstream = std::stringstream(line);
		if(line == "")
			break;
		float weightW;
		m_Stringstream >> filler >> tempWeight.x >> tempWeight.y >> tempWeight.z >> weightW;

		std::getline(p_Input, line);
		m_Stringstream = std::stringstream(line);
		m_Stringstream >> filler >> tempJoint.x >> tempJoint.y >> tempJoint.z >> tempJoint.w;
		
		if(tempJoint.w != 0)
		{
			float weightSum = tempWeight.x + tempWeight.y + tempWeight.z + weightW;
			tempWeight.x /= weightSum;
			tempWeight.y /= weightSum;
			tempWeight.z /= weightSum;
		}
		else
		{
			float weightSum = tempWeight.x + tempWeight.y + tempWeight.z + 0;
			tempWeight.x /= weightSum;
			tempWeight.y /= weightSum;
			tempWeight.z /= weightSum;
		}
		m_WeightsList.push_back(std::make_pair(tempWeight, tempJoint));
	}
}

void ModelLoader::readHierarchy(std::istream& p_Input)
{
	std::string line, filler;
	Joint tempJointStruct;
	while(std::getline(p_Input, line))
	{
		m_Stringstream = std::stringstream(line);
		if(line == "")
			break;
		m_Stringstream >> tempJointStruct.m_JointName >> tempJointStruct.m_ID >> filler >> tempJointStruct.m_Parent;
		m_ListOfJoints.push_back(tempJointStruct);
	}
}

void ModelLoader::readJointOffset(std::istream& p_Input)
{
	std::string line, filler;
	DirectX::XMFLOAT4X4 tempMat4x4;
	int i = 0;
	while(std::getline(p_Input, line))
	{
		m_Stringstream = std::stringstream(line);
		if(line == "")
			break;
		m_Stringstream >> filler 
			>> tempMat4x4._11 >> tempMat4x4._12 >> tempMat4x4._13 >> tempMat4x4._14 
			>> tempMat4x4._21 >> tempMat4x4._22	>> tempMat4x4._23 >> tempMat4x4._24
			>> tempMat4x4._31 >> tempMat4x4._32 >> tempMat4x4._33 >> tempMat4x4._34
			>> tempMat4x4._41 >> tempMat4x4._42 >> tempMat4x4._43 >> tempMat4x4._44;
		m_ListOfJoints.at(i).m_JointOffsetMatrix = tempMat4x4;
		i++;
	}
}

void ModelLoader::readAnimation(std::istream& p_Input)
{
	std::string line, filler;
	KeyFrame tempKeyFrame;
	int i = 0;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> m_Start;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> m_End;
	std::getline(p_Input, line);
	m_Stringstream = std::stringstream(line);
	m_Stringstream >> filler >> m_NumberOfFrames;
	std::getline(p_Input, line);
	while(std::getline(p_Input, line))
	{
		if(line == "")
			break;
		for(int j = 0; j < m_NumberOfFrames; j++)
		{
			std::getline(p_Input, line);
			m_Stringstream = std::stringstream(line);
			m_Stringstream >> filler 
				>> tempKeyFrame.m_Trans.x >> tempKeyFrame.m_Trans.y >> tempKeyFrame.m_Trans.z >> filler
				>> tempKeyFrame.m_Rot.x >> tempKeyFrame.m_Rot.y >> tempKeyFrame.m_Rot.z >> tempKeyFrame.m_Rot.w >> filler
				>> tempKeyFrame.m_Scale.x >> tempKeyFrame.m_Scale.y >> tempKeyFrame.m_Scale.z;
			m_ListOfJoints.at(i).m_JointAnimation.push_back(tempKeyFrame);
		}
		i++;
	}
}

void ModelLoader::printOutResourceInfo(std::string p_ResourceListLocation)
{
	tinyxml2::XMLDocument resource;
	bool found = false;
	p_ResourceListLocation.append("\\Resources.xml");
	tinyxml2::XMLError error = resource.LoadFile(p_ResourceListLocation.c_str());
	tinyxml2::XMLElement* element;
	if(error == tinyxml2::XML_ERROR_EMPTY_DOCUMENT || error == tinyxml2::XML_ERROR_FILE_NOT_FOUND)
	{
		element = resource.NewElement("Resources");
		resource.InsertFirstChild(element);
	}
	else if(error != tinyxml2::XML_NO_ERROR)
	{
		std::cerr << "Loading xml file failed, error code: " << error;
	}
	else
	{
		element = resource.FirstChildElement("Resources");
	}

	tinyxml2::XMLElement* resourceType = searchForElement(resource, element, "ResourceType", "Type", "model"); 
	tinyxml2::XMLElement* leafNode = searchForElement(resource, resourceType, "Resource", "Name", m_MeshName);

	std::string path = "assets/models/"; 
	path.append(m_MeshName);
	path.append(".btx");
	printPath(leafNode, path);

	if(m_Collidable)
	{
		resourceType = searchForElement(resource, element, "ResourceType", "Type", "volume");
		leafNode = searchForElement(resource, resourceType, "Resource", "Name", m_MeshName);
		std::string path = "assets/volumes/CB_"; 
		path.append(m_MeshName);
		path.append(".txc");
		printPath(leafNode, path);
	}
	if(m_WeightsList.size() > 0)
	{
		resourceType = searchForElement(resource, element, "ResourceType", "Type", "animation");
		leafNode = searchForElement(resource, resourceType, "Resource", "Name", m_MeshName);
		std::string path = "assets/animations/"; 
		path.append(m_MeshName);
		path.append(".atx");
		printPath(leafNode, path);
	}

	error = resource.SaveFile(p_ResourceListLocation.c_str());
	if(error != tinyxml2::XML_NO_ERROR)
	{
		std::cout << "Could not save file: " << p_ResourceListLocation << std::endl;
	}
}

void ModelLoader::printPath(tinyxml2::XMLElement* p_Ele, std::string p_Path)
{
	p_Ele->SetAttribute("Path", p_Path.c_str());
}

tinyxml2::XMLElement* ModelLoader::searchForElement(tinyxml2::XMLDocument& p_Doc, tinyxml2::XMLElement* p_Parent, std::string p_ElementName, std::string p_Attribute, std::string p_AttributeValue)
{
	tinyxml2::XMLElement* child;
	for(child = p_Parent->FirstChildElement(p_ElementName.c_str()); child; child = child->NextSiblingElement(p_ElementName.c_str()))
	{
		if(child->Attribute(p_Attribute.c_str(), p_AttributeValue.c_str()))
			break;
	}

	if (!child)
	{
		child = p_Doc.NewElement(p_ElementName.c_str());
		child->SetAttribute(p_Attribute.c_str(), p_AttributeValue.c_str());
		p_Parent->InsertEndChild(child);
	}
	return child;
}

const std::vector<DirectX::XMFLOAT3>& ModelLoader::getVertices() const
{
	return m_Vertices;
}

bool ModelLoader::getTransparent() const
{
	return m_Transparent;
}

bool ModelLoader::getCollidable() const
{
	return m_Collidable;
}

const std::vector<std::vector<ModelLoader::IndexDesc>>& ModelLoader::getIndices() const
{
	return m_IndexPerMaterial;
}

const std::vector<ModelLoader::Material>& ModelLoader::getMaterial() const
{
	return m_Material;
}

const std::vector<DirectX::XMFLOAT3>& ModelLoader::getNormals() const
{
	return m_Normals;
}

const std::vector<DirectX::XMFLOAT3>& ModelLoader::getTangents() const
{
	return m_Tangents;
}

const std::vector<DirectX::XMFLOAT2>& ModelLoader::getTextureCoords() const
{
	return m_TextureCoord;
}

const std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMINT4>>& ModelLoader::getWeightsList() const
{
	return m_WeightsList;
}

const std::vector<ModelLoader::Joint>& ModelLoader::getListOfJoints() const
{
	return m_ListOfJoints;
}

float ModelLoader::getAnimationStartValue() const
{
	return m_Start;
}

float ModelLoader::getAnimationEndValue() const
{
	return m_End;
}

int ModelLoader::getNumberOfFrames() const
{
	return m_NumberOfFrames;
}

std::string ModelLoader::getMeshName() const
{
	return m_MeshName;
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
	m_MeshName = "";
	m_NumberOfVertices = 0;
	m_NumberOfTriangles = 0;
}