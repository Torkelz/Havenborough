#include "Server.h"

#include "../../Client/Source/Logger.h"

const float Server::m_PlayerSphereRadius = 50.f;

Server::Server()
	:	m_LastActorId(0),
		m_RemoveBox(false),
		m_PulseObject(false)
{
}

void Server::initialize()
{
	m_Running = false;
	
	m_Network = INetwork::createNetwork();
	m_Network->initialize();
	m_Network->createServer(31415);
	m_Network->setClientConnectedCallback(&Server::clientConnected, this);
	m_Network->setClientDisconnectedCallback(&Server::clientDisconnected, this);
	m_Network->startServer(3);

	m_Lobby.reset(new Lobby());
}

void Server::run()
{
	m_Running = true;
	m_UpdateThread = std::thread(&Server::updateClients, this);
}

void Server::shutdown()
{
	std::lock_guard<std::mutex> lock(m_UserLock);

	m_Running = false;
	m_Lobby.reset();

	m_UpdateThread.join();
	
	m_Network->setClientConnectedCallback(nullptr, nullptr);
	m_Network->setClientDisconnectedCallback(nullptr, nullptr);
	INetwork::deleteNetwork(m_Network);
}

std::vector<std::string> Server::getUserNames()
{
	std::vector<std::string> names;

	std::lock_guard<std::mutex> lock(m_UserLock);

	for (unsigned int i = 0; i < m_Players.size(); i++)
	{
		names.push_back(std::to_string((intptr_t)m_Players[i].m_Connection->getConnection()));
	}

	return names;
}

void Server::sendTestData()
{
	m_RemoveBox = true;
}

void Server::sendPulseObject()
{
	m_PulseObject = true;
}

void Server::clientConnected(IConnectionController* p_Connection, void* p_UserData)
{
	Logger::log(Logger::Level::INFO, "Client connected");

	Server* obj = static_cast<Server*>(p_UserData);

	User::ptr user(new User(p_Connection));

	std::vector<std::string> descriptions;
	std::vector<const char*> cDescriptions;
	std::vector<ObjectInstance> instances;

	for (const auto& box : obj->m_Boxes)
	{
		descriptions.push_back(obj->getBoxDescription(box));
		cDescriptions.push_back(descriptions.back().c_str());
		instances.push_back(obj->getBoxInstance(box, descriptions.size() - 1));
	}

	for (const auto& player : obj->m_Players)
	{
		descriptions.push_back(obj->getPlayerBoxDescription(player.m_PlayerBox));
		cDescriptions.push_back(descriptions.back().c_str());
		instances.push_back(obj->getBoxInstance(player.m_PlayerBox, descriptions.size() - 1));
	}

	user->getConnection()->sendCreateObjects(cDescriptions.data(), cDescriptions.size(), instances.data(), instances.size());

	descriptions.clear();
	cDescriptions.clear();
	instances.clear();

	TestBox newBox =
	{
		++obj->m_LastActorId,
		Vector3(0.f, 0.f, 0.f),
		Vector3(0.f, 0.f, 0.f),
		Vector3(0.f, 0.f, 0.f),
		Vector3(0.f, 0.f, 0.f),
		Vector3(500.f, 200.f + (float)obj->m_LastActorId * 100.f, 400.f),
		(float)obj->m_LastActorId * 100.f,
		3.14f / 10.f,
		0.f
	};
	obj->updateBox(newBox, 0.f);

	descriptions.push_back(obj->getBoxDescription(newBox));
	cDescriptions.push_back(descriptions.back().c_str());
	instances.push_back(obj->getBoxInstance(newBox, descriptions.size() - 1));

	Vector3 position(500.f - obj->m_LastActorId * 200.f, obj->m_PlayerSphereRadius, 600.f);

	TestPlayer newPlayer =
	{
		user,
		{
			++obj->m_LastActorId,
			position,
			Vector3(0.f, 0.f, 0.f),
			Vector3(0.f, 0.f, 0.f),
			Vector3(0.f, 0.f, 0.f)
		}
	};

	descriptions.push_back(obj->getPlayerBoxDescription(newPlayer.m_PlayerBox));
	cDescriptions.push_back(descriptions.back().c_str());
	instances.push_back(obj->getBoxInstance(newPlayer.m_PlayerBox, descriptions.size() - 1));

	std::lock_guard<std::mutex> lock(obj->m_UserLock);
	obj->m_Players.push_back(newPlayer);

	for(auto& player : obj->m_Players)
	{
		player.m_Connection->getConnection()->sendCreateObjects(cDescriptions.data(), cDescriptions.size(), instances.data(), instances.size());
	}

	obj->m_Boxes.push_back(newBox);

	user->getConnection()->sendAssignPlayer(newPlayer.m_PlayerBox.actorId);
}

void Server::clientDisconnected(IConnectionController* p_Connection, void* p_UserData)
{
	Logger::log(Logger::Level::INFO, "Client disconnected");

	Server* obj = static_cast<Server*>(p_UserData);

	std::lock_guard<std::mutex> lock(obj->m_UserLock);

	bool playerRemoved = false;
	TestPlayer removedPlayer;

	for(unsigned int i = 0; i < obj->m_Players.size(); i++)
	{
		if(obj->m_Players[i].m_Connection->getConnection() == p_Connection)
		{
			removedPlayer = obj->m_Players[i];
			playerRemoved = true;

			obj->m_Players.erase(obj->m_Players.begin() + i);
			break;
		}
	}

	if (playerRemoved)
	{
		for (auto& player : obj->m_Players)
		{
			player.m_Connection->getConnection()->sendRemoveObjects(&removedPlayer.m_PlayerBox.actorId, 1);
		}
	}
}

void Server::updateBox(TestBox& p_Box, float p_DeltaTime)
{
	p_Box.circleRotation += p_Box.circleRotationSpeed * p_DeltaTime;
	p_Box.position.x = p_Box.circleCenter.y + cos(p_Box.circleRotation) * p_Box.circleRadius;
	p_Box.position.y = p_Box.circleCenter.x;
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

void Server::updatePlayerBox(TestPlayerBox& p_Box, float p_DeltaTime)
{
	p_Box.rotation = p_Box.rotation + p_Box.rotationVelocity * p_DeltaTime;
}

UpdateObjectData Server::getUpdateData(const TestBox& p_Box)
{
	UpdateObjectData data =
	{
		{ p_Box.position.x, p_Box.position.y, p_Box.position.z },
		{ p_Box.velocity.x, p_Box.velocity.y, p_Box.velocity.z },
		{ p_Box.rotation.x, p_Box.rotation.y, p_Box.rotation.z },
		{ p_Box.rotationVelocity.x, p_Box.rotationVelocity.y, p_Box.rotationVelocity.z },
		p_Box.actorId
	};

	return data;
}

UpdateObjectData Server::getUpdateData(const TestPlayerBox& p_Box)
{
	UpdateObjectData data =
	{
		{ p_Box.position.x, p_Box.position.y, p_Box.position.z },
		{ p_Box.velocity.x, p_Box.velocity.y, p_Box.velocity.z },
		{ p_Box.rotation.x + 3.1415f, 0.f, p_Box.rotation.z },
		{ p_Box.rotationVelocity.x, p_Box.rotationVelocity.y, p_Box.rotationVelocity.z },
		p_Box.actorId
	};

	return data;
}

void Server::pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, const Vector3& p_Vec)
{
	p_Printer.OpenElement(p_ElementName.c_str());
	p_Printer.PushAttribute("x", p_Vec.x);
	p_Printer.PushAttribute("y", p_Vec.y);
	p_Printer.PushAttribute("z", p_Vec.z);
	p_Printer.CloseElement();
}

std::string Server::getBoxDescription(const TestBox& p_Box)
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

std::string Server::getPlayerBoxDescription(const TestPlayerBox& p_Box)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Movement");
	pushVector(printer, "Velocity", p_Box.velocity);
	pushVector(printer, "RotationalVelocity", p_Box.rotationVelocity);
	printer.CloseElement();
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "WITCH");
	static const Vector3 scale(1.f, 1.f, 1.f);
	pushVector(printer, "Scale", scale);
	printer.CloseElement();
	//printer.OpenElement("SpherePhysics");
	//printer.PushAttribute("Immovable", true);
	//printer.PushAttribute("Radius", playerSphereRadius);
	//printer.CloseElement();
	printer.OpenElement("Pulse");
	printer.PushAttribute("Length", 0.5f);
	printer.PushAttribute("Strength", 0.5f);
	printer.CloseElement();
	printer.CloseElement();
	return std::string(printer.CStr());
}

ObjectInstance Server::getBoxInstance(const TestBox& p_Box, uint16_t p_DescIdx)
{
	ObjectInstance inst =
	{
		{ p_Box.position.x, p_Box.position.y, p_Box.position.z },
		{ p_Box.rotation.x, p_Box.rotation.y, p_Box.rotation.z },
		p_DescIdx,
		p_Box.actorId
	};

	return inst;
}

ObjectInstance Server::getBoxInstance(const TestPlayerBox& p_Box, uint16_t p_DescIdx)
{
	ObjectInstance inst =
	{
		{ p_Box.position.x, p_Box.position.y, p_Box.position.z },
		{ p_Box.rotation.x, p_Box.rotation.y, p_Box.rotation.z },
		p_DescIdx,
		p_Box.actorId
	};

	return inst;
}

void Server::removeLastBox()
{
	if (m_Boxes.empty())
	{
		return;
	}

	const uint16_t objectsToRemove[] = { m_Boxes.back().actorId };
	m_Boxes.pop_back();

	std::lock_guard<std::mutex> lock(m_UserLock);
	for(auto& player : m_Players)
	{
		player.m_Connection->getConnection()->sendRemoveObjects(objectsToRemove, 1);
	}
}

void Server::pulse()
{
	if (m_Boxes.empty())
	{
		return;
	}

	const uint16_t object = m_Boxes.front().actorId;

	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Action");
	printer.OpenElement("Pulse");
	printer.CloseElement();
	printer.CloseElement();

	std::lock_guard<std::mutex> lock(m_UserLock);
	for(auto& player : m_Players)
	{
		player.m_Connection->getConnection()->sendObjectAction(object, printer.CStr());
	}
}

void Server::handlePackages()
{
	std::lock_guard<std::mutex> lock(m_UserLock);
	for(auto& player : m_Players)
	{
		IConnectionController* con = player.m_Connection->getConnection();

		unsigned int numPackages = con->getNumPackages();
		for (unsigned int i = 0; i < numPackages; ++i)
		{
			Package package = con->getPackage(i);
			PackageType type = con->getPackageType(package);

			switch (type)
			{
			case PackageType::PLAYER_CONTROL:
				{
					PlayerControlData playerControlData = con->getPlayerControlData(package);
					player.m_PlayerBox.position.x = playerControlData.m_Velocity[0];
					player.m_PlayerBox.position.y = playerControlData.m_Velocity[1];
					player.m_PlayerBox.position.z = playerControlData.m_Velocity[2];
					//player.m_PlayerBox.velocity.x = playerControlData.m_Velocity[0];
					//player.m_PlayerBox.velocity.y = playerControlData.m_Velocity[1];
					//player.m_PlayerBox.velocity.z = playerControlData.m_Velocity[2];
					player.m_PlayerBox.rotation.x = playerControlData.m_Rotation[0];
					player.m_PlayerBox.rotation.y = playerControlData.m_Rotation[1];
					player.m_PlayerBox.rotation.z = playerControlData.m_Rotation[2];
				}
				break;

			default:
				std::string msg("Received unhandled package of type " + std::to_string((uint16_t)type));
				Logger::log(Logger::Level::WARNING, msg);
				break;
			}
		}

		con->clearPackages(numPackages);
	}
}

void Server::updateClients()
{
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previousTime;
	float deltaTime = 0.001f;

	while (m_Running)
	{
		handlePackages();

		if (m_RemoveBox)
		{
			removeLastBox();
			m_RemoveBox = false;
		}

		if (m_PulseObject)
		{
			pulse();
			m_PulseObject = false;
		}

		std::vector<UpdateObjectData> data;

		for (auto& box : m_Boxes)
		{
			updateBox(box, deltaTime);
			data.push_back(getUpdateData(box));
		}

		for (auto& player : m_Players)
		{
			updatePlayerBox(player.m_PlayerBox, deltaTime);
			data.push_back(getUpdateData(player.m_PlayerBox));
		}

		{
			std::lock_guard<std::mutex> lock(m_UserLock);
			for (auto& player : m_Players)
			{
				player.m_Connection->getConnection()->sendUpdateObjects(data.data(), data.size(), nullptr, 0);
			}
		}
		
		previousTime = currentTime;
		currentTime = std::chrono::high_resolution_clock::now();
		const std::chrono::high_resolution_clock::duration frameTime = currentTime - previousTime;

		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(frameTime).count();

		static const std::chrono::milliseconds sleepDuration(20);
		std::this_thread::sleep_for(sleepDuration - frameTime);
	}
}