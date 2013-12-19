#include "Level.h"

Level::Level(IGraphics* p_Graphics, ResourceManager* p_Resource, IPhysics* p_Physics)
{
	m_Graphic = p_Graphics;
	m_Resource = p_Resource;
	m_Physics = p_Physics;
}

Level::~Level()
{
}

void Level::releaseLevel()
{
	for(int i : m_ResourceID)
	{
		m_Resource->releaseResource(i);
	}
	for(int j : m_BVResourceID)
	{
		m_Resource->releaseResource(j);
	}
	m_ResourceID.clear();
	m_ResourceID.shrink_to_fit();
	m_BVResourceID.clear();
	m_BVResourceID.shrink_to_fit();
	m_LevelData.clear();
	m_LevelData.shrink_to_fit();
	m_LevelCollisionData.clear();
	m_LevelCollisionData.shrink_to_fit();
	m_Graphic = nullptr;
	m_Resource = nullptr;
}
bool Level::loadLevel(std::string p_LevelFilePath, std::string p_CollisionFilePath)
{
	if(!m_LevelLoader.loadBinaryFile(p_LevelFilePath))
	{
		return false;
	}
	m_Graphic->createShader("DefaultShader", L"../../Graphics/Source/DeferredShaders/GeometryPass.hlsl",
							"VS,PS","5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	m_LevelData = m_LevelLoader.getModelData();
	for(unsigned int i = 0; i < m_LevelData.size(); i++)
	{
		m_ResourceID.push_back(m_Resource->loadResource("model", m_LevelData.at(i).m_MeshName));
		m_Graphic->linkShaderToModel("DefaultShader", m_LevelData.at(i).m_MeshName.c_str());
		for(unsigned int j = 0; j < m_LevelData.at(i).m_Translation.size(); j++)
		{
			m_DrawID.push_back(m_Graphic->createModelInstance(m_LevelData.at(i).m_MeshName.c_str()));
			DirectX::XMFLOAT3 translation, rotation, scale;
			translation = m_LevelData.at(i).m_Translation.at(j);
			rotation = m_LevelData.at(i).m_Rotation.at(j);
			scale = m_LevelData.at(i).m_Scale.at(j);
			m_Graphic->setModelPosition(m_DrawID.back(), translation.x, translation.y, translation.z);
			m_Graphic->setModelRotation(m_DrawID.back(), rotation.x, rotation.y, rotation.z);
			m_Graphic->setModelScale(m_DrawID.back(), scale.x, scale.y, scale.z);
		}
	}
	//This will be implemented at a later stage when physics has what it takes!
	if(!m_CollisionLoader.loadBinaryFile(p_CollisionFilePath))
	{
		return false;
	}
	m_LevelCollisionData = m_CollisionLoader.getModelData();
	for(unsigned int i = 0; i < m_LevelCollisionData.size(); i++)
	{
		m_BVResourceID.push_back(m_Resource->loadResource("volume", m_LevelCollisionData.at(i).m_MeshName));

		//for(unsigned int j = 0; m_LevelCollisionData.at(i).m_Translation.size(); j++)
		//{
		//	//m_DrawID.push_back(m_Graphic->createModelInstance(m_LevelData.at(i).m_MeshName.c_str()));
		//	
		//	DirectX::XMFLOAT3 translation, rotation, scale;
		//	translation = m_LevelData.at(i).m_Translation.at(j);
		//	rotation = m_LevelData.at(i).m_Rotation.at(j);
		//	scale = m_LevelData.at(i).m_Scale.at(j);
		//	m_Physics->setBVPosition(m_DrawID.back(), translation.z, translation.y, translation.x);
		//	m_Physics->setBVRotation(m_DrawID.back(), rotation.z, rotation.y, rotation.x);
		//	m_Physics->setBVScale(m_DrawID.back(), scale.z, scale.y, scale.x);
		//}
	}


	return true;
}

void Level::drawLevel()
{
	for(unsigned int i = 0; i < m_DrawID.size(); i++)
	{
		m_Graphic->renderModel(m_DrawID.at(i));
	}
}