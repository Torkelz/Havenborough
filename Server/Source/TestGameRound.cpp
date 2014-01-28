#include "TestGameRound.h"

#include <Components.h>
#include <Logger.h>

#include <algorithm>

const float TestGameRound::m_PlayerSphereRadius = 50.f;

void TestGameRound::setup()
{
	for (size_t i = 0; i < m_Players.size(); ++i)
	{
		uint16_t actorId = i + 200;

		TestBox newBox =
		{
			actorId,
			Vector3(0.f, 0.f, 0.f),
			Vector3(0.f, 0.f, 0.f),
			Vector3(0.f, 0.f, 0.f),
			Vector3(0.f, 0.f, 0.f),
			Vector3(500.f, 200.f + i * 100.f, 400.f),
			(float)(i + 3) * 100.f,
			3.14f / 10.f,
			0.f
		};
		updateBox(newBox, 0.f);

		m_Boxes.push_back(newBox);

		Vector3 position(500.f - i * 200.f, m_PlayerSphereRadius + 400.f, 600.f);

		Actor::ptr actor = m_ActorFactory->createPlayerActor(position);
		m_Players[i].setActor(actor);
		m_Actors.push_back(actor);
	}
}

void TestGameRound::sendLevel()
{
	std::vector<std::string> descriptions;
	std::vector<const char*> cDescriptions;
	std::vector<ObjectInstance> instances;

	for (const auto& box : m_Boxes)
	{
		descriptions.push_back(getBoxDescription(box));
		cDescriptions.push_back(descriptions.back().c_str());
		instances.push_back(getBoxInstance(box, descriptions.size() - 1));
	}

	for (const auto& player : m_Players)
	{
		Actor::ptr actor = player.getActor().lock();
		if (actor)
		{
			descriptions.push_back(m_ActorFactory->getPlayerActorDescription(actor->getPosition()));
			cDescriptions.push_back(descriptions.back().c_str());

			ObjectInstance inst =
			{
				actor->getPosition(),
				actor->getRotation(),
				descriptions.size() - 1,
				actor->getId()
			};

			instances.push_back(inst);
		}
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
	cDescriptions.push_back(descriptions.back().c_str());
	ObjectInstance lightData =
	{
		Vector3(0.f, 0.f, 0.f),
		Vector3(0.f, 0.f, 0.f),
		descriptions.size() - 1,
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
				user->getConnection()->sendCreateObjects(cDescriptions.data(), cDescriptions.size(), instances.data(), instances.size());
				user->getConnection()->sendAssignPlayer(actor->getId());
			}
		}
	}
}

void TestGameRound::updateLogic(float p_DeltaTime)
{
	for (auto& box : m_Boxes)
	{
		updateBox(box, p_DeltaTime);
	}

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
		data.push_back(getUpdateData(box));
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

void TestGameRound::updateBox(TestBox& p_Box, float p_DeltaTime)
{
	p_Box.circleRotation += p_Box.circleRotationSpeed * p_DeltaTime;
	p_Box.position.x = p_Box.circleCenter.x + cos(p_Box.circleRotation) * p_Box.circleRadius;
	p_Box.position.y = p_Box.circleCenter.y;
	p_Box.position.z = p_Box.circleCenter.z - sin(p_Box.circleRotation) * p_Box.circleRadius;
	p_Box.velocity.x = -sin(p_Box.circleRotation) * p_Box.circleRadius * p_Box.circleRotationSpeed;
	p_Box.velocity.y = 0.f;
	p_Box.velocity.z = -cos(p_Box.circleRotation) * p_Box.circleRadius * p_Box.circleRotationSpeed;
	p_Box.rotation.x = p_Box.circleRotation;
	p_Box.rotation.y = 0.f;
	p_Box.rotation.z = p_Box.circleRotation;
	p_Box.rotationVelocity.x = p_Box.circleRotationSpeed;
	p_Box.rotationVelocity.y = 0.f;
	p_Box.rotationVelocity.z = p_Box.circleRotationSpeed;
}

UpdateObjectData TestGameRound::getUpdateData(const TestBox& p_Box)
{
	UpdateObjectData data =
	{
		p_Box.position,
		p_Box.velocity,
		p_Box.rotation,
		p_Box.rotationVelocity,
		p_Box.actorId
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

std::string TestGameRound::getBoxDescription(const TestBox& p_Box)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Movement");
	pushVector(printer, "Velocity", p_Box.velocity);
	pushVector(printer, "RotationalVelocity", p_Box.rotationVelocity);
	printer.CloseElement();
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "BOX");
	static const Vector3 scale(100.f, 100.f, 100.f);
	pushVector(printer, "Scale", scale);
	printer.CloseElement();
	printer.OpenElement("OBBPhysics");
	pushVector(printer, "Halfsize", scale * 0.5f);
	printer.CloseElement();
	printer.OpenElement("Pulse");
	printer.PushAttribute("Length", 0.5f);
	printer.PushAttribute("Strength", 0.5f);
	printer.CloseElement();
	printer.CloseElement();
	return std::string(printer.CStr());
}

ObjectInstance TestGameRound::getBoxInstance(const TestBox& p_Box, uint16_t p_DescIdx)
{
	ObjectInstance inst =
	{
		p_Box.position,
		p_Box.rotation,
		p_DescIdx,
		p_Box.actorId
	};

	return inst;
}
