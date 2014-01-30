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

	m_Actors.push_back(m_ActorFactory->createDirectionalLight(Vector3(0.f, -1.f, 0.f), Vector3(1.f, 1.f, 1.f)));
	m_Actors.push_back(m_ActorFactory->createDirectionalLight(Vector3(0.f, -1.f, 0.f), Vector3(1.0f, 1.0f, 1.0f)));
	m_Actors.push_back(m_ActorFactory->createSpotLight(Vector3(-1000.f,500.f,0.f), Vector3(0,0,-1),
		Vector2(cosf(3.14f/12),cosf(3.14f/4)), 2000.f, Vector3(0.f,1.f,0.f)));
	m_Actors.push_back(m_ActorFactory->createPointLight(Vector3(0.f,0.f,0.f), 2000.f, Vector3(1.f,1.f,1.f)));
	m_Actors.push_back(m_ActorFactory->createPointLight(Vector3(0.f, 3000.f, 3000.f), 2000000.f, Vector3(0.5f, 0.5f, 0.5f)));
	m_Actors.push_back(m_ActorFactory->createPointLight(Vector3(0.f, 0.f, 3000.f), 2000000.f, Vector3(0.5f, 0.5f, 0.5f)));

	m_FileLoader.reset(new LevelBinaryLoader);
	m_FileLoader->loadBinaryFile(m_FilePath);
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
