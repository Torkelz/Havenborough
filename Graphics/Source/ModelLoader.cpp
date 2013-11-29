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
		tempFace = Face();
		stringstream = std::stringstream(line);
		stringstream >> key >> std::ws;
		if(strcmp(key.c_str(), "Materials") == 0)
		{
			stringstream >> m_NumberOfMaterials;
		}
		else if(strcmp(key.c_str(), "#Vertices") == 0)
		{
			stringstream >> m_NumberOfVertices;
		}
		else if(strcmp(key.c_str(), "#Triangles") == 0)
		{
			stringstream >> m_NumberOfTriangles;
		}
		else if(strcmp(key.c_str(), "Material:") == 0)
		{
			stringstream >> tempMaterial.m_MaterialID;
			readMaterial++;
		}
		else if(strcmp(key.c_str(), "DiffuseMap:") == 0)
		{
			stringstream >> tempMaterial.m_DiffuseMap;
			readMaterial++;
		}
		else if(strcmp(key.c_str(), "NormalMap:") == 0)
		{
			stringstream >> tempMaterial.m_NormalMap;
			readMaterial++;
		}
		else if(strcmp(key.c_str(), "SpecularMap:") == 0)
		{
			stringstream >> tempMaterial.m_SpecularMap;
			readMaterial++;
		}
		else if(strcmp(key.c_str(), "v") == 0)
		{
			stringstream >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
			m_Vertex.push_back(tempFlaot3);
		}
		else if(strcmp(key.c_str(), "t") == 0)
		{
			stringstream >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
			m_Tangents.push_back(tempFlaot3);
		}
		else if(strcmp(key.c_str(), "n") == 0)
		{
			stringstream >> tempFlaot3.x >> tempFlaot3.y >> tempFlaot3.z;
			m_Normals.push_back(tempFlaot3);
		}
		else if(strcmp(key.c_str(), "uv") == 0)
		{
			stringstream >> tempFlaot3.x >> tempFlaot3.y;
			m_TextureCoord.push_back(DirectX::XMFLOAT2(tempFlaot3.x,tempFlaot3.y));
		}
		else if(strcmp(key.c_str(), "face:") == 0)
		{
			stringstream >> key >> filler >> tempFace.m_MaterialID;
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

		if(readMaterial == 4)
		{
			m_Material.push_back(tempMaterial);
			readMaterial = 0;
		}
	}

}