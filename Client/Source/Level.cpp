#include "Level.h"
#include "InstanceBinaryLoader.h"
#include "boost\filesystem.hpp"
#include <XMLHelper.h>

Level::Level(ResourceManager* p_Resources, ActorFactory* p_ActorFactory)
{
	m_Resources = p_Resources;
	m_ActorFactory = p_ActorFactory;

	m_StartPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_GoalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Level::~Level()
{
	m_Resources = nullptr;

	m_StartPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_GoalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Level::releaseLevel()
{
	m_Resources = nullptr;
}

bool Level::loadLevel(std::istream& p_LevelData, ActorList::ptr p_ActorOut)
{
	InstanceBinaryLoader levelLoader;
	boost::filesystem::path collisionFolder("assets/volumes/edge");
	levelLoader.readStreamData(p_LevelData);	

	std::vector<InstanceBinaryLoader::ModelData> m_LevelData = levelLoader.getModelData();
	for(unsigned int i = 0; i < m_LevelData.size(); i++)
	{
		InstanceBinaryLoader::ModelData& model = m_LevelData.at(i);

		ActorFactory::InstanceModel instModel;
		instModel.meshName = model.m_MeshName;

		std::vector<ActorFactory::InstanceBoundingVolume> volumes;
		std::vector<ActorFactory::InstanceEdgeBox> edges;

		if(model.m_CollideAble)
		{
			ActorFactory::InstanceBoundingVolume volume;
			volume.meshName = model.m_MeshName;
			volumes.push_back(volume);

			boost::filesystem::path EBPath = collisionFolder/("EB_" + model.m_MeshName + ".btxe");
			if(boost::filesystem::exists(EBPath))
			{
				InstanceBinaryLoader EBLoader;
				EBLoader.loadBinaryFile(EBPath.string());
				const InstanceBinaryLoader::ModelData& eb = EBLoader.getModelData()[0];
				for(unsigned int k = 0; k < EBLoader.getModelData()[0].m_Translation.size(); k++)
				{
					ActorFactory::InstanceEdgeBox edge;
					edge.halfsize = eb.m_Scale[k];
					edge.halfsize = edge.halfsize * 0.5f;
					edge.offsetPosition = eb.m_Translation[k];
					edge.offsetRotation = eb.m_Rotation[k];
					edges.push_back(edge);
				}
			}
		}

		for(unsigned int j = 0; j < model.m_Translation.size(); j++)
		{
			instModel.position = model.m_Translation[j];
			instModel.rotation = model.m_Rotation[j];
			instModel.scale = model.m_Scale[j];

			if (model.m_CollideAble)
			{
				volumes[0].scale = model.m_Scale[j];
			}

			p_ActorOut->addActor(m_ActorFactory->createInstanceActor(instModel, volumes, edges));
		}
	}
	
	p_LevelData.seekg(0, p_LevelData.beg);
		
	Actor::ptr directionalActor;
	Actor::ptr pointActor;
	Actor::ptr spotActor;
	for (const auto& directionalLight : levelLoader.getDirectionalLightData())
	{
		directionalActor = m_ActorFactory->createDirectionalLight(directionalLight.m_Direction, directionalLight.m_Color, directionalLight.m_Intensity);
		p_ActorOut->addActor(directionalActor);
	}
	for (const auto& pointLight : levelLoader.getPointLightData())
	{
		pointActor = m_ActorFactory->createPointLight(pointLight.m_Translation, pointLight.m_Intensity * 5000, pointLight.m_Color);
		p_ActorOut->addActor(pointActor);
	}
	Vector2 minMaxAngle;
	for (const auto& spotLight : levelLoader.getSpotLightData())
	{
		minMaxAngle.x = cosf(spotLight.m_ConeAngle); minMaxAngle.y = cosf(spotLight.m_ConeAngle + spotLight.m_PenumbraAngle);
		spotActor = m_ActorFactory->createSpotLight(spotLight.m_Translation, spotLight.m_Direction, minMaxAngle, spotLight.m_Intensity * 5000, spotLight.m_Color);
		p_ActorOut->addActor(spotActor);
	}

	return true;
}

const Vector3 &Level::getStartPosition(void) const
{
	return m_StartPosition;
}

void Level::setStartPosition(const Vector3 &p_StartPosition)
{
	m_StartPosition = p_StartPosition;
}

const Vector3 &Level::getGoalPosition(void) const
{
	return m_GoalPosition;
}

void Level::setGoalPosition(const Vector3 &p_GoalPosition)
{
	m_GoalPosition = p_GoalPosition;
}
