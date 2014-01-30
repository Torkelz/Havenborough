#include "FileGameRound.h"

#include <Components.h>
#include <Logger.h>
#include <XMLHelper.h>

#include <fstream>
#include <sstream>

using namespace DirectX;

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
	if(m_Physics->getHitDataSize() > 0)
	{
		for(int i = m_Physics->getHitDataSize()-1 ; i >= 0; i--)
		{
			HitData hit = m_Physics->getHitDataAt(i);
			if(!m_CheckpointSystem.reachedFinishLine() && 
				m_CheckpointSystem.getCurrentCheckpointBodyHandle() == hit.collisionVictim)
			{				
				m_SendHitData.push_back(std::make_pair(m_Players[hit.collider-1].getUser(), m_Actors[hit.collisionVictim-1]));
			}
		}
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
	if(m_SendHitData.size() > 0)
	for(unsigned int i = 0; i < m_SendHitData.size(); i++)
	{
		User::ptr user = m_SendHitData[i].first.lock();
		if (user)
		{
			Actor::ptr actor = m_SendHitData[i].second.lock();
			if (actor)
			{
				Actor::Id id = actor->getId();
				user->getConnection()->sendRemoveObjects(&id, 1);
				tinyxml2::XMLPrinter printer;
				printer.OpenElement("ObjectUpdate");
				printer.PushAttribute("ActorId", id-1);
				printer.PushAttribute("Type", "Color");
				pushColor(printer, "SetColor", m_CheckpointSystem.changeCheckpoint());
				printer.CloseElement();
				const char* info = printer.CStr();
				user->getConnection()->sendUpdateObjects(NULL, 0, &info, 1);
			}
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

	Actor::Id playerActorId = actor->getId();

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