#include "TestGameRound.h"

#include <Components.h>
#include <Logger.h>
#include <XMLHelper.h>

#include <algorithm>
#include <sstream>

const float TestGameRound::m_PlayerSphereRadius = 50.f;

void TestGameRound::setup()
{
	for (size_t i = 0; i < m_Players.size(); ++i)
	{
		Actor::ptr box = m_ActorFactory->createCircleBox(
			Vector3(500.f, 200.f + i * 100.f, 400.f),
			(float)(i + 3) * 100.f);
		m_Actors.push_back(box);
		m_Boxes.push_back(box);

		Vector3 position(500.f + i * 200.f, m_PlayerSphereRadius + 400.f, 500.f);

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

	m_Actors.push_back(m_ActorFactory->createCheckPointActor(Vector3(4850.0f, 0.0f, -2528.0f), Vector3(1.0f, 10.0f, 1.0f)));
	m_Actors.push_back(m_ActorFactory->createCheckPointActor(Vector3(-1000.0f, 0.0f, -1000.0f), Vector3(1.0f, 10.0f, 1.0f)));
	m_Actors.push_back(m_ActorFactory->createCheckPointActor(Vector3(-1000.0f, 0.0f, 1000.0f), Vector3(1.0f, 10.0f, 1.0f)));
	m_Actors.push_back(m_ActorFactory->createCheckPointActor(Vector3(1000.0f, 0.0f, 1000.0f), Vector3(1.0f, 10.0f, 1.0f)));
	m_Actors.push_back(m_ActorFactory->createCheckPointActor(Vector3(1000.0f, 0.0f, -1000.0f), Vector3(1.0f, 10.0f, 1.0f)));

	m_Actors.push_back(m_ActorFactory->createParticles(Vector3(4850.0f, 0.0f, -2528.0f), "TestParticle"));
	m_Actors.push_back(m_ActorFactory->createParticles(Vector3(-1000.0f, 0.0f, -1000.0f), "TestParticle"));
	m_Actors.push_back(m_ActorFactory->createParticles(Vector3(-1000.0f, 0.0f, 1000.0f), "TestParticle"));
	m_Actors.push_back(m_ActorFactory->createParticles(Vector3(1000.0f, 0.0f, 1000.0f), "TestParticle"));
	m_Actors.push_back(m_ActorFactory->createParticles(Vector3(1000.0f, 0.0f, -1000.0f), "TestParticle"));
}

void TestGameRound::sendLevel()
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

	for(auto& player : m_Players)
	{
		User::ptr user = player.getUser().lock();
		if (user)
		{
			Actor::ptr actor = player.getActor().lock();
			if (actor)
			{
				user->getConnection()->sendCreateObjects(instances.data(), instances.size());
				user->getConnection()->sendLevelData("", 0);
				user->getConnection()->sendAssignPlayer(actor->getId());
			}
		}
	}
}

void TestGameRound::updateLogic(float p_DeltaTime)
{
	for (auto& actor : m_Actors)
	{
		actor->onUpdate(p_DeltaTime);
	}
}

void TestGameRound::sendUpdates()
{
	std::vector<UpdateObjectData> data;
	std::vector<std::string> extra;
	std::vector<const char*> extraC;

	for (auto& box : m_Boxes)
	{
		Actor::ptr actor = box.lock();
		if (actor)
		{
			data.push_back(getUpdateData(actor));
		}
	}

	for (auto& player : m_Players)
	{
		data.push_back(getUpdateData(player));
		extra.push_back(getExtraData(player));
		extraC.push_back(extra.back().c_str());
	}

	for (auto& player : m_Players)
	{
		User::ptr user = player.getUser().lock();
		if (user)
		{
			user->getConnection()->sendUpdateObjects(data.data(), data.size(), extraC.data(), extraC.size());
		}
	}
}

void TestGameRound::playerDisconnected(Player& p_DisconnectedPlayer)
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

UpdateObjectData TestGameRound::getUpdateData(const Actor::ptr p_Box)
{
	std::shared_ptr<MovementInterface> movement = p_Box->getComponent<MovementInterface>(MovementInterface::m_ComponentId).lock();

	Vector3 velocity(0.f, 0.f, 0.f);
	Vector3 rotVelocity(0.f, 0.f, 0.f);
	if (movement)
	{
		velocity = movement->getVelocity();
		rotVelocity = movement->getRotationalVelocity();
	}

	UpdateObjectData data =
	{
		p_Box->getPosition(),
		velocity,
		p_Box->getRotation(),
		rotVelocity,
		p_Box->getId()
	};

	return data;
}

UpdateObjectData TestGameRound::getUpdateData(const Player& p_Player)
{
	Actor::ptr actor = p_Player.getActor().lock();
	
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

std::string TestGameRound::getExtraData(const Player& p_Player)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("ObjectUpdate");
	printer.PushAttribute("ActorId", p_Player.getActor().lock()->getId());
	p_Player.getActor().lock()->getComponent<LookInterface>(LookInterface::m_ComponentId).lock()->serialize(printer);
	printer.CloseElement();

	return printer.CStr();
}
