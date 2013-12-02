#include "ModelLoader.h"

ModelLoader::ModelLoader()
{
}

ModelLoader::~ModelLoader()
{
	m_Index.clear();
	m_Normals.clear();
	m_Vertex.clear();
	m_Tangents.clear();
	m_TextureCoord.clear();	
}

bool ModelLoader::loadFile(std::string p_Filename)
{
	
	std::ifstream input(p_Filename.c_str(),std::ifstream::in);
	std::string line, key, filler;
	int readMaterial = 0;
	int tempInt;
	Material tempMaterial;
	Face tempFace;
	DirectX::XMFLOAT3 tempFlaot3;
	std::stringstream stringstream;

	if(!input)
	{
		return false;
	}

	while (!input.eof() && std::getline(input, line))
	{

		key = "";
		stringstream = std::stringstream(line);
		stringstream >> key >> std::ws;
		if(strcmp(key.c_str(), "*m3d-File-Header") == 0)
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
		else if(strcmp(key.c_str(), "*Materials") == 0)
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
		else if(strcmp(key.c_str(), "*Vertices") == 0)
		{
			for(int i = 0; i < m_NumberOfVertices; i++)
			{	
				std::getline(input, line);
				stringstream = std::stringstream(line);
				stringstream >> key >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
				m_Vertex.push_back(tempFlaot3);
			}
		}
		else if(strcmp(key.c_str(), "*Tangets") == 0)
		{
			while(std::getline(input, line))
			{
				stringstream = std::stringstream(line);
				if(strcmp(line.c_str(),""))
					break;
				stringstream >> key >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
				m_Tangents.push_back(tempFlaot3);
			}
		}
		else if(strcmp(key.c_str(), "*Normals") == 0)
		{
			while(std::getline(input, line))
			{
				stringstream = std::stringstream(line);
				if(strcmp(line.c_str(),""))
					break;
				stringstream >> key >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
				m_Normals.push_back(tempFlaot3);
			}
		}
		else if(strcmp(key.c_str(), "*UVCOORDS") == 0)
		{
			while(std::getline(input, line))
			{
				stringstream = std::stringstream(line);
				if(strcmp(line.c_str(),""))
					break;
				stringstream >> key >> tempFlaot3.x >> tempFlaot3.y;
				m_TextureCoord.push_back(DirectX::XMFLOAT2(tempFlaot3.x,tempFlaot3.y));
			}
		}
		else if(strcmp(key.c_str(), "*FACES") == 0)
		{
			while(std::getline(input,line))
			{
				tempFace = Face();
				stringstream = std::stringstream(line);
				if(strcmp(line.c_str(),""))
					break;
				stringstream >> key >> key >> filler >> tempFace.m_MaterialID;
				std::getline(input, line);
				stringstream = std::stringstream(line);
				for(int i = 0; i < atoi(key.c_str()); i++)
				{
					stringstream >> tempInt >> filler;
					tempFace.m_Vertex.push_back(m_Vertex.at(tempInt));
					stringstream >> tempInt >> filler;
					tempFace.m_Tangents.push_back(m_Tangents.at(tempInt));
					stringstream >> tempInt >> filler;
					tempFace.m_Normals.push_back(m_Normals.at(tempInt));
					stringstream >> tempInt >> filler;	
					tempFace.m_TextureCoord.push_back(m_TextureCoord.at(tempInt));
				}
				m_Index.push_back(tempFace);
			}
			m_IndexPerMaterial.push_back(m_Index);
		}
	}
	m_Index.clear();
	m_Index.shrink_to_fit();
	m_Normals.clear();
	m_Normals.shrink_to_fit();
	m_Vertex.clear();
	m_Vertex.shrink_to_fit();
	m_Tangents.clear();
	m_Tangents.shrink_to_fit();
	m_TextureCoord.clear();
	m_TextureCoord.shrink_to_fit();
	return true;
}