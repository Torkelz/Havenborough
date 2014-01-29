#include "FileGameRound.h"

#include <Components.h>
#include <Logger.h>
#include <XMLHelper.h>

#include <fstream>
#include <sstream>

void FileGameRound::setup()
{
	for (size_t i = 0; i < m_Players.size(); ++i)
	{
		Vector3 position(500.f - i * 200.f, 50 + 400.f, 500.f);

		Actor::ptr actor = m_ActorFactory->createPlayerActor(position);
		m_Players[i].setActor(actor);
		m_Actors.push_back(actor);
	}

	m_FileLoader.reset(new LevelBinaryLoader);
	m_FileLoader->loadBinaryFile(m_FilePath);

	Actor::ptr directionalActor;
	Actor::ptr pointActor;
	Actor::ptr spotActor;
	for (const auto& directionalLight : m_FileLoader->getDirectionalLightData())
	{
		directionalActor = m_ActorFactory->createDirectionalLight(directionalLight.m_Direction, directionalLight.m_Color);
		m_Actors.push_back(directionalActor);
	}
	for (const auto& pointLight : m_FileLoader->getPointLightData())
	{
		pointActor = m_ActorFactory->createPointLight(pointLight.m_Translation, pointLight.m_Intensity * 10000, pointLight.m_Color);
		m_Actors.push_back(pointActor);
	}
	Vector2 minMaxAngle;
	for (const auto& spotLight : m_FileLoader->getSpotLightData())
	{
		minMaxAngle.x = cosf(spotLight.m_ConeAngle); minMaxAngle.y = cosf(spotLight.m_ConeAngle + spotLight.m_PenumbraAngle);
		spotActor = m_ActorFactory->createSpotLight(spotLight.m_Translation, spotLight.m_Direction, minMaxAngle, spotLight.m_Intensity * 10000, spotLight.m_Color);
		m_Actors.push_back(spotActor);
	}

	std::vector<LevelBinaryLoader::CheckPointStruct> checkpoints = m_FileLoader->getCheckPointData();
	std::sort(checkpoints.begin(), checkpoints.end(),
		[] (const LevelBinaryLoader::CheckPointStruct& p_Left, const LevelBinaryLoader::CheckPointStruct& p_Right)
		{
			return p_Left.m_Number > p_Right.m_Number;
		});
	
	static const Vector3 checkpointScale(1.f, 10.f, 1.f);

	Actor::ptr checkActor = m_ActorFactory->createCheckPointActor(m_FileLoader->getCheckPointEnd(), checkpointScale);
	m_CheckpointSystem.addCheckpoint(checkActor);
	m_Actors.push_back(checkActor);
	for (const auto& checkpoint : checkpoints)
	{
		checkActor = m_ActorFactory->createCheckPointActor(checkpoint.m_Translation, checkpointScale);
		m_CheckpointSystem.addCheckpoint(checkActor);
		m_Actors.push_back(checkActor);
	}
	checkActor = m_ActorFactory->createCheckPointActor(m_FileLoader->getCheckPointStart(), checkpointScale);
	m_CheckpointSystem.addCheckpoint(checkActor);
	m_Actors.push_back(checkActor);
}

void FileGameRound::setFilePath(std::string p_Filepath)
{
	m_FilePath = p_Filepath;
}

void FileGameRound::sendLevel()
{
	std::vector<std::string> descriptions;
	std::vector<ObjectInstance> instances;

	for (const auto& actor : m_Actors)
	{
		std::ostringstream descStream;
		actor->serialize(descStream);
		descriptions.push_back(descStream.str());
		ObjectInstance inst = 
		{
			descriptions.back().c_str(),
			actor->getId()
		};
		instances.push_back(inst);
	}

	std::string stream = m_FileLoader->getDataStream();
	m_FileLoader.reset();

	for (auto& player : m_Players)
	{
		User::ptr user = player.getUser().lock();
		if (user)
		{
			Actor::ptr actor = player.getActor().lock();
			if(actor)
			{
				user->getConnection()->sendCreateObjects(instances.data(), instances.size());
				user->getConnection()->sendLevelData(stream.c_str(), stream.size());
				user->getConnection()->sendAssignPlayer(actor->getId());
			}
		}
	}
}

void FileGameRound::updateLogic(float p_DeltaTime)
{
	for (auto& actor : m_Actors)
	{
		actor->onUpdate(p_DeltaTime);
	}
}

void FileGameRound::sendUpdates()
{
	std::vector<UpdateObjectData> data;

	for (auto& player : m_Players)
	{
		data.push_back(getUpdateData(player));
	}

	for (auto& player : m_Players)
	{
		User::ptr user = player.getUser().lock();
		if (user)
		{
			user->getConnection()->sendUpdateObjects(data.data(), data.size(), nullptr, 0);
		}
	}
}

void FileGameRound::playerDisconnected(Player& p_DisconnectedPlayer)
{
	Actor::ptr actor = p_DisconnectedPlayer.getActor().lock();
	if (!actor)
	{
		return;
	}

	uint16_t playerActorId = actor->getId();

	for (auto& player : m_Players)
	{
		User::ptr user = player.getUser().lock();
		if (user)
		{
			user->getConnection()->sendRemoveObjects(&playerActorId, 1);
		}
	}
}

UpdateObjectData FileGameRound::getUpdateData(const Player& p_Player)
{
	Actor::ptr actor = p_Player.getActor().lock();
	
	if (!actor)
	{
		throw CommonException("Player missing actor", __LINE__, __FILE__);
	}

	std::shared_ptr<MovementInterface> movement = actor->getComponent<MovementInterface>(MovementInterface::m_ComponentId).lock();

	Vector3 velocity(0.f, 0.f, 0.f);
	Vector3 rotVelocity(0.f, 0.f, 0.f);
	if (movement)
	{
		velocity = movement->getVelocity();
		rotVelocity = movement->getRotationalVelocity();
	}

	UpdateObjectData data =
	{
		actor->getPosition(),
		velocity,
		actor->getRotation(),
		rotVelocity,
		actor->getId()
	};

	return data;
}
