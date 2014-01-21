#include "LevelConverter.h"

LevelConverter::LevelConverter()
{
	m_Header.m_NumberOfModels = 0;
}

LevelConverter::~LevelConverter()
{

}

void LevelConverter::clear()
{
	m_Header.m_NumberOfModels = 0;
}

bool LevelConverter::writeFile(std::string p_FilePath)
{
	std::ofstream output(p_FilePath, std::ostream::out | std::ostream::binary);
	if(!output)
	{
		return false;
	}
	if(m_Header.m_NumberOfModels != 0)
	{
		createHeader(&output);
		createLevel(&output);
	}
	else
	{
		return false;
	}
	return true;
}

void LevelConverter::createHeader(std::ostream* p_Output)
{
	intToByte(m_Header.m_NumberOfModels, p_Output);
}

void LevelConverter::createLevel(std::ostream* p_Output)
{
	std::vector<ModelData> level;
	ModelData model;
	bool written = false;
	for(int i = 0; i < m_LevelDataSize; i++)
	{
		model = ModelData();
		for(unsigned int j = 0; j < level.size(); j++)
		{
			if(level.at(j).m_MeshName == m_LevelData->at(i).m_MeshName)
			{
				DirectX::XMFLOAT3 translation = m_LevelData->at(i).m_Translation;
				translation.x *= -1.f;
				level.at(j).m_Translation.push_back(translation);

				DirectX::XMFLOAT3 rotation = m_LevelData->at(i).m_Rotation;
				rotation.x *= -1.f;
				rotation.z *= -1.f;
				level.at(j).m_Rotation.push_back(rotation);
				level.at(j).m_Scale.push_back(m_LevelData->at(i).m_Scale);
				written = true;
				break;
			}	
		}
		if(written != true)
		{
			DirectX::XMFLOAT3 translation = m_LevelData->at(i).m_Translation;
			translation.x *= -1.f;
			model.m_Translation.push_back(translation);

			DirectX::XMFLOAT3 rotation = m_LevelData->at(i).m_Rotation;
			rotation.x *= -1.f;
			rotation.z *= -1.f;
			model.m_Rotation.push_back(rotation);
			model.m_Scale.push_back(m_LevelData->at(i).m_Scale);
			model.m_MeshName = m_LevelData->at(i).m_MeshName;
			level.push_back(model);
		}
		written = false;
	}
	intToByte(level.size(), p_Output);
	for(unsigned int i = 0; i < level.size(); i++)
	{
		stringToByte(level.at(i).m_MeshName, p_Output);
		intToByte(level.at(i).m_Translation.size(), p_Output);
		p_Output->write(reinterpret_cast<const char*>(level.at(i).m_Translation.data()), sizeof(DirectX::XMFLOAT3) * level.at(i).m_Translation.size());
		intToByte(level.at(i).m_Rotation.size(), p_Output);
		p_Output->write(reinterpret_cast<const char*>(level.at(i).m_Rotation.data()), sizeof(DirectX::XMFLOAT3) * level.at(i).m_Rotation.size());
		intToByte(level.at(i).m_Scale.size(), p_Output);
		p_Output->write(reinterpret_cast<const char*>(level.at(i).m_Scale.data()), sizeof(DirectX::XMFLOAT3) * level.at(i).m_Scale.size());
	}
}

void LevelConverter::stringToByte(std::string p_String, std::ostream* p_Output)
{
	unsigned int size = p_String.size();
	p_Output->write(reinterpret_cast<const char*>(&size), sizeof(unsigned int));
	p_Output->write(p_String.data(), p_String.size());
}

void LevelConverter::intToByte(int p_Int, std::ostream* p_Output)
{
	p_Output->write(reinterpret_cast<const char*>(&p_Int), sizeof(p_Int));
}

void LevelConverter::setLevelHead(LevelLoader::LevelHeader p_Header)
{
	m_Header = p_Header;
}

void LevelConverter::setLevelModelList(const std::vector<LevelLoader::ModelStruct>* p_LevelModelList)
{
	m_LevelData = p_LevelModelList;
	m_LevelDataSize = m_LevelData->size();
}

void LevelConverter::setLevelDirectionalLightList(const std::vector<LevelLoader::DirectionalLight>* p_LevelDirectionalLightList)
{
	m_LevelDirectionalLightList = p_LevelDirectionalLightList;
}

void LevelConverter::setLevelPointLightList(const std::vector<LevelLoader::PointLight>* p_LevelPointLightList)
{
	m_LevelPointLightList = p_LevelPointLightList;
}

void LevelConverter::setLevelSpotLightList(const std::vector<LevelLoader::SpotLight>* p_LevelSpotLightList)
{
	m_LevelSpotLightList = p_LevelSpotLightList;
}