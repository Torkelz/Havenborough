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
		m_Players[i]->setActor(actor);
		m_Actors.push_back(actor);
	}
	m_FileLoader.reset(new InstanceBinaryLoader);
	m_FileLoader->loadBinaryFile(m_FilePath);

	std::vector<InstanceBinaryLoader::CheckPointStruct> checkpoints = m_FileLoader->getCheckPointData();
	std::sort(checkpoints.begin(), checkpoints.end(),
		[] (const InstanceBinaryLoader::CheckPointStruct& p_Left, const InstanceBinaryLoader::CheckPointStruct& p_Right)
		{
			return p_Left.m_Number > p_Right.m_Number;
		});
	
	static const Vector3 checkpointScale(1.f, 10.f, 1.f);

	std::vector<Actor::ptr> checkpointList;
	checkpointList.push_back(m_ActorFactory->createCheckPointActor(m_FileLoader->getCheckPointEnd(), checkpointScale));
	for (const auto& checkpoint : checkpoints)
	{
		checkpointList.push_back(m_ActorFactory->createCheckPointActor(checkpoint.m_Translation, checkpointScale));
	}
	checkpointList.push_back(m_ActorFactory->createCheckPointActor(m_FileLoader->getCheckPointStart(), checkpointScale));

	for (const auto& checkpoint : checkpointList)
	{
		m_Actors.push_back(checkpoint);
		for(auto& player : m_Players)
		{
			player->addCheckpoint(checkpoint);
		}
	}

	m_GoalCount = 0;
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
		User::ptr user = player->getUser().lock();
		if (user)
		{
			Actor::ptr actor = player->getActor().lock();
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
	for(int i = m_Physics->getHitDataSize()-1 ; i >= 0; i--)
	{
		HitData hit = m_Physics->getHitDataAt(i);
		if (m_Players.size() != 0)
		{
			Player::ptr player = findPlayer(hit.collider);
			Actor::ptr victim = findActor(hit.collisionVictim);

			if (player && victim)
			{
				if(!player->reachedFinishLine())
				{
					if(player->getCurrentCheckpointBodyHandle() == hit.collisionVictim)
					{
						m_SendHitData.push_back(std::make_pair(player, victim));
						player->changeCheckpoint();
					}
				}
			}
		}
	}
}

void FileGameRound::handleExtraPackage(Player::ptr p_Player, Package p_Package)
{
	User::ptr user = p_Player->getUser().lock();
	if (!user)
	{
		return;
	}

	IConnectionController* con = user->getConnection();
	PackageType type = con->getPackageType(p_Package);

	switch (type)
	{
	case PackageType::THROW_SPELL:
		{
			Actor::ptr playerActor = p_Player->getActor().lock();
			if (!playerActor)
			{
				break;
			}

			Actor::ptr spellActor = m_ActorFactory->createSpell(
				con->getThrowSpellName(p_Package),
				playerActor->getId(),
				con->getThrowSpellDirection(p_Package),
				con->getThrowSpellStartPosition(p_Package));

			std::ostringstream outStream;
			spellActor->serialize(outStream);
			std::string spellDescription = outStream.str();

			ObjectInstance spellInstance;
			spellInstance.m_Id = spellActor->getId();
			spellInstance.m_Description = spellDescription.c_str();

			for (auto& player : m_Players)
			{
				if (player == p_Player)
				{
					continue;
				}

				User::ptr otherUser = player->getUser().lock();
				if (!otherUser)
				{
					continue;
				}

				otherUser->getConnection()->sendCreateObjects(&spellInstance, 1);
			}
		}
		break;

	case PackageType::OBJECT_ACTION:
		{
			Actor::Id actor = con->getObjectActionId(p_Package);
			const char* action = con->getObjectActionAction(p_Package);

			for (auto& player : m_Players)
			{
				if (player == p_Player)
				{
					continue;
				}

				User::ptr otherUser = player->getUser().lock();
				if (!otherUser)
				{
					continue;
				}

				otherUser->getConnection()->sendObjectAction(actor, action);
			}
		}
		break;

	default:
		GameRound::handleExtraPackage(p_Player, p_Package);
		break;
	}
}

void FileGameRound::sendUpdates()
{
	std::vector<UpdateObjectData> data;
	bool goalReached = false;
	std::vector<std::string> extra;
	std::vector<const char*> extraC;
	for (auto& player : m_Players)
	{
		data.push_back(getUpdateData(player));
		extra.push_back(getExtraData(player));
		extraC.push_back(extra.back().c_str());
	}
	for (auto& player : m_Players)
	{
		User::ptr user = player->getUser().lock();
		if (user)
		{
			user->getConnection()->sendUpdateObjects(data.data(), data.size(), extraC.data(), extraC.size());
		}
	}
	Actor::Id id;
	if(m_GoalCount >= 0)
	{
		for(unsigned int i = 0; i < m_SendHitData.size(); i++)
		{
			Player::ptr player = m_SendHitData[i].first;
			User::ptr user = player->getUser().lock();
			if (user)
			{
				Actor::ptr actor = m_SendHitData[i].second.lock();
				if (actor)
				{
					id = actor->getId();
					if(!m_SendHitData[i].first->reachedFinishLine())
					{
						user->getConnection()->sendRemoveObjects(&id, 1);
						user->getConnection()->sendSetSpawnPosition(actor->getPosition());
						tinyxml2::XMLPrinter printer;
						printer.OpenElement("ObjectUpdate");
						printer.PushAttribute("ActorId", id-1);
						printer.PushAttribute("Type", "Color");
						pushColor(printer, "SetColor", m_SendHitData[i].first->getCurrentCheckpointColor());
						printer.CloseElement();
						const char* info = printer.CStr();
						user->getConnection()->sendUpdateObjects(NULL, 0, &info, 1);
					}
					else
					{
						user->getConnection()->sendRemoveObjects(&id, 1);
						m_GoalCount++;
						tinyxml2::XMLPrinter printer;
						printer.OpenElement("GameResult");
						printer.PushAttribute("Type", "Position");
						printer.PushAttribute("Place", m_GoalCount);
						printer.CloseElement();
						const char* info = printer.CStr();
						user->getConnection()->sendGameResult(&info, 1);
						m_ResultList.push_back(player->getActor().lock()->getId());
					}
				}
			}
			m_SendHitData.clear();
		}
		if(m_Players.size() == m_GoalCount)
		{
			tinyxml2::XMLPrinter printer;
			printer.OpenElement("GameResult");
			printer.PushAttribute("Type", "Result");
			printer.OpenElement("ResultList");
			printer.PushAttribute("VectorSize", m_Players.size());
			for(unsigned int i = 0; i < m_ResultList.size(); i++)
			{
				printer.OpenElement("Place");
				printer.PushAttribute("Player", m_ResultList[i]);
				printer.CloseElement();
			}
			printer.CloseElement();
			printer.CloseElement();
			const char* info = printer.CStr();
			for(auto& player : m_Players)
			{
				player->getUser().lock()->getConnection()->sendGameResult(&info, 1);
			}
			m_GoalCount = -1;
		}
	}
}

void FileGameRound::playerDisconnected(Player::ptr p_DisconnectedPlayer)
{
	Actor::ptr actor = p_DisconnectedPlayer->getActor().lock();
	if (!actor)
	{
		return;
	}

	Actor::Id playerActorId = actor->getId();

	for (auto& player : m_Players)
	{
		User::ptr user = player->getUser().lock();
		if (user)
		{
			user->getConnection()->sendRemoveObjects(&playerActorId, 1);
		}
	}
}

UpdateObjectData FileGameRound::getUpdateData(const Player::ptr p_Player)
{
	Actor::ptr actor = p_Player->getActor().lock();
	
	if (!actor)
	{
		throw CommonException("Player missing actor", __LINE__, __FILE__);
	}

	std::shared_ptr<PhysicsInterface> physComp = actor->getComponent<PhysicsInterface>(PhysicsInterface::m_ComponentId).lock();

	Vector3 velocity(0.f, 0.f, 0.f);
	Vector3 rotVelocity(0.f, 0.f, 0.f);

	if (physComp)
	{
		velocity = physComp->getVelocity();
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

std::string FileGameRound::getExtraData(const Player::ptr p_Player)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("ObjectUpdate");
	printer.PushAttribute("ActorId", p_Player->getActor().lock()->getId());
	printer.PushAttribute("Type", "Look");
	p_Player->getActor().lock()->getComponent<LookInterface>(LookInterface::m_ComponentId).lock()->serialize(printer);
	printer.CloseElement();

	return printer.CStr();
}

Player::ptr FileGameRound::findPlayer(BodyHandle p_Body)
{
	auto playerIt = std::find_if(m_Players.begin(), m_Players.end(),
	[&p_Body] (Player::ptr p_Player)
	{
		Actor::ptr actor = p_Player->getActor().lock();
					
		return actor && actor->getBodyHandles()[0] == p_Body;
	});

	if (playerIt == m_Players.end())
	{
		return Player::ptr();
	}
	else
	{
		return *playerIt;
	}
}

Actor::ptr FileGameRound::findActor(BodyHandle p_Body)
{
	auto actorIt = std::find_if(m_Actors.begin(), m_Actors.end(),
	[&p_Body] (Actor::ptr p_Actor)
	{
		if (!p_Actor)
		{
			return false;
		}

		for (auto& body : p_Actor->getBodyHandles())
		{
			if (body == p_Body)
			{
				return true;
			}
		}
		return false;
	});

	if (actorIt == m_Actors.end())
	{
		return Actor::ptr();
	}
	else
	{
		return *actorIt;
	}
}
