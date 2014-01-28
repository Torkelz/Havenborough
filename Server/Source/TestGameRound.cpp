#include "TestGameRound.h"

#include <Components.h>
#include <Logger.h>

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

	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Light");
	printer.PushAttribute("Type", "Directional");
	pushVector(printer, "Direction", Vector3(0.f, -1.f, 0.f));
	static const Vector3 color(1.f, 1.f, 1.f);
	printer.OpenElement("Color");
	printer.PushAttribute("r", color.x);
	printer.PushAttribute("g", color.y);
	printer.PushAttribute("b", color.z);
	printer.CloseElement();
	printer.CloseElement();
	printer.CloseElement();

	descriptions.push_back(printer.CStr());
	ObjectInstance lightData =
	{
		descriptions.back().c_str(),
		0
	};
	instances.push_back(lightData);

	for(auto& player : m_Players)
	{
		User::ptr user = player.getUser().lock();
		if (user)
		{
			Actor::ptr actor = player.getActor().lock();
			if (actor)
			{
				user->getConnection()->sendCreateObjects(instances.data(), instances.size());
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

void TestGameRound::pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, const Vector3& p_Vec)
{
	p_Printer.OpenElement(p_ElementName.c_str());
	p_Printer.PushAttribute("x", p_Vec.x);
	p_Printer.PushAttribute("y", p_Vec.y);
	p_Printer.PushAttribute("z", p_Vec.z);
	p_Printer.CloseElement();
}
