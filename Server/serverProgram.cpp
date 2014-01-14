#include <INetwork.h>
#include "../../Client/Source/Logger.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <tinyxml2.h>

std::mutex g_ControllerLock;

bool running = false;
std::thread updateThread;

struct TestBox
{
	uint16_t actorId;

	float position[3];
	float velocity[3];
	float rotation[3];
	float rotationVelocity[3];

	float circleCenter[3];
	float circleRadius;
	float circleRotationSpeed;
	float circleRotation;
};

static const float playerSphereRadius = 50.f;

struct TestPlayerBox
{
	uint16_t actorId;

	float position[3];
	float velocity[3];
	float rotation[3];
	float rotationVelocity[3];
};

struct TestPlayer
{
	IConnectionController* m_Connection;

	TestPlayerBox m_PlayerBox;
};

uint16_t lastActorId = 0;
std::vector<TestBox> boxes;
std::vector<TestPlayer> players;

void updateBox(TestBox& p_Box, float p_DeltaTime)
{
	p_Box.circleRotation += p_Box.circleRotationSpeed * p_DeltaTime;
	p_Box.position[0] = p_Box.circleCenter[0] + cos(p_Box.circleRotation) * p_Box.circleRadius;
	p_Box.position[1] = p_Box.circleCenter[1];
	p_Box.position[2] = p_Box.circleCenter[2] - sin(p_Box.circleRotation) * p_Box.circleRadius;
	p_Box.velocity[0] = -sin(p_Box.circleRotation) * p_Box.circleRadius * p_Box.circleRotationSpeed;
	p_Box.velocity[1] = 0.f;
	p_Box.velocity[2] = -cos(p_Box.circleRotation) * p_Box.circleRadius * p_Box.circleRotationSpeed;
	p_Box.rotation[0] = p_Box.circleRotation;
	p_Box.rotation[1] = 0.f;
	p_Box.rotation[2] = p_Box.circleRotation;
	p_Box.rotationVelocity[0] = p_Box.circleRotationSpeed;
	p_Box.rotationVelocity[1] = 0.f;
	p_Box.rotationVelocity[2] = p_Box.circleRotationSpeed;
}

void updatePlayerBox(TestPlayerBox& p_Box, float p_DeltaTime)
{
	p_Box.position[0] += p_Box.velocity[0] * p_DeltaTime;
	p_Box.position[1] += p_Box.velocity[1] * p_DeltaTime;
	p_Box.position[2] += p_Box.velocity[2] * p_DeltaTime;
	p_Box.rotation[0] += p_Box.rotationVelocity[0] * p_DeltaTime;
	p_Box.rotation[1] += p_Box.rotationVelocity[1] * p_DeltaTime;
	p_Box.rotation[2] += p_Box.rotationVelocity[2] * p_DeltaTime;
}

UpdateObjectData getUpdateData(const TestBox& p_Box)
{
	UpdateObjectData data =
	{
		{ p_Box.position[0], p_Box.position[1], p_Box.position[2] },
		{ p_Box.velocity[0], p_Box.velocity[1], p_Box.velocity[2] },
		{ p_Box.rotation[0], p_Box.rotation[1], p_Box.rotation[2] },
		{ p_Box.rotationVelocity[0], p_Box.rotationVelocity[1], p_Box.rotationVelocity[2] },
		p_Box.actorId
	};

	return data;
}

UpdateObjectData getUpdateData(const TestPlayerBox& p_Box)
{
	UpdateObjectData data =
	{
		{ p_Box.position[0], p_Box.position[1], p_Box.position[2] },
		{ p_Box.velocity[0], p_Box.velocity[1], p_Box.velocity[2] },
		{ p_Box.rotation[0], p_Box.rotation[1], p_Box.rotation[2] },
		{ p_Box.rotationVelocity[0], p_Box.rotationVelocity[1], p_Box.rotationVelocity[2] },
		p_Box.actorId
	};

	return data;
}

void pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, const float p_Vec[3])
{
	p_Printer.OpenElement(p_ElementName.c_str());
	p_Printer.PushAttribute("x", p_Vec[0]);
	p_Printer.PushAttribute("y", p_Vec[1]);
	p_Printer.PushAttribute("z", p_Vec[2]);
	p_Printer.CloseElement();
}

std::string getBoxDescription(const TestBox& p_Box)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Movement");
	pushVector(printer, "Velocity", p_Box.velocity);
	pushVector(printer, "RotationalVelocity", p_Box.rotationVelocity);
	printer.CloseElement();
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "BOX");
	static const float scale[3] = {100.f, 100.f, 100.f};
	pushVector(printer, "Scale", scale);
	printer.CloseElement();
	printer.OpenElement("OBBPhysics");
	static const float halfsize[3] = {50.f, 50.f, 50.f};
	pushVector(printer, "Halfsize", halfsize);
	printer.CloseElement();
	printer.OpenElement("Pulse");
	printer.PushAttribute("Length", 0.5f);
	printer.PushAttribute("Strength", 0.5f);
	printer.CloseElement();
	printer.CloseElement();
	return std::string(printer.CStr());
}

std::string getPlayerBoxDescription(const TestPlayerBox& p_Box)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Movement");
	pushVector(printer, "Velocity", p_Box.velocity);
	pushVector(printer, "RotationalVelocity", p_Box.rotationVelocity);
	printer.CloseElement();
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "BOX");
	static const float scale[3] = {playerSphereRadius, playerSphereRadius, playerSphereRadius};
	pushVector(printer, "Scale", scale);
	printer.CloseElement();
	printer.OpenElement("SpherePhysics");
	printer.PushAttribute("Immovable", true);
	printer.PushAttribute("Radius", playerSphereRadius);
	printer.CloseElement();
	printer.OpenElement("Pulse");
	printer.PushAttribute("Length", 0.5f);
	printer.PushAttribute("Strength", 0.5f);
	printer.CloseElement();
	printer.CloseElement();
	return std::string(printer.CStr());
}

ObjectInstance getBoxInstance(const TestBox& p_Box, uint16_t p_DescIdx)
{
	ObjectInstance inst =
	{
		{ p_Box.position[0], p_Box.position[1], p_Box.position[2] },
		{ p_Box.rotation[0], p_Box.rotation[1], p_Box.rotation[2] },
		p_DescIdx,
		p_Box.actorId
	};

	return inst;
}

ObjectInstance getBoxInstance(const TestPlayerBox& p_Box, uint16_t p_DescIdx)
{
	ObjectInstance inst =
	{
		{ p_Box.position[0], p_Box.position[1], p_Box.position[2] },
		{ p_Box.rotation[0], p_Box.rotation[1], p_Box.rotation[2] },
		p_DescIdx,
		p_Box.actorId
	};

	return inst;
}

bool removeBox = false;

void removeLastBox()
{
	if (boxes.empty())
	{
		return;
	}

	const uint16_t objectsToRemove[] = { boxes.back().actorId };
	boxes.pop_back();

	std::lock_guard<std::mutex> lock(g_ControllerLock);
	for(auto& player : players)
	{
		player.m_Connection->sendRemoveObjects(objectsToRemove, 1);
	}
}

bool pulseObject = false;

void pulse()
{
	if (boxes.empty())
	{
		return;
	}

	const uint16_t object = boxes.front().actorId;

	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Action");
	printer.OpenElement("Pulse");
	printer.CloseElement();
	printer.CloseElement();

	std::lock_guard<std::mutex> lock(g_ControllerLock);
	for(auto& player : players)
	{
		player.m_Connection->sendObjectAction(object, printer.CStr());
	}
}

void handlePackages()
{
	std::lock_guard<std::mutex> lock(g_ControllerLock);
	for(auto& player : players)
	{
		IConnectionController* con = player.m_Connection;

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
					player.m_PlayerBox.velocity[0] = playerControlData.m_Velocity[0];
					player.m_PlayerBox.velocity[1] = playerControlData.m_Velocity[1];
					player.m_PlayerBox.velocity[2] = playerControlData.m_Velocity[2];
					player.m_PlayerBox.rotation[0] = playerControlData.m_Rotation[0];
					player.m_PlayerBox.rotation[1] = playerControlData.m_Rotation[1];
					player.m_PlayerBox.rotation[2] = playerControlData.m_Rotation[2];
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

void updateClients()
{
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previousTime;
	float deltaTime = 0.001f;

	while (running)
	{
		handlePackages();

		if (removeBox)
		{
			removeLastBox();
			removeBox = false;
		}

		if (pulseObject)
		{
			pulse();
			pulseObject = false;
		}

		std::vector<UpdateObjectData> data;

		for (auto& box : boxes)
		{
			updateBox(box, deltaTime);
			data.push_back(getUpdateData(box));
		}

		for (auto& player : players)
		{
			updatePlayerBox(player.m_PlayerBox, deltaTime);
			data.push_back(getUpdateData(player.m_PlayerBox));
		}

		{
			std::lock_guard<std::mutex> lock(g_ControllerLock);
			for (auto& player : players)
			{
				player.m_Connection->sendUpdateObjects(data.data(), data.size(), nullptr, 0);
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

void clientConnected(IConnectionController* p_Connection, void* /*p_UserData*/)
{
	Logger::log(Logger::Level::INFO, "Client connected");

	std::vector<std::string> descriptions;
	std::vector<const char*> cDescriptions;
	std::vector<ObjectInstance> instances;

	for (const auto& box : boxes)
	{
		descriptions.push_back(getBoxDescription(box));
		cDescriptions.push_back(descriptions.back().c_str());
		instances.push_back(getBoxInstance(box, descriptions.size() - 1));
	}

	for (const auto& player : players)
	{
		descriptions.push_back(getPlayerBoxDescription(player.m_PlayerBox));
		cDescriptions.push_back(descriptions.back().c_str());
		instances.push_back(getBoxInstance(player.m_PlayerBox, descriptions.size() - 1));
	}

	p_Connection->sendCreateObjects(cDescriptions.data(), cDescriptions.size(), instances.data(), instances.size());


	descriptions.clear();
	cDescriptions.clear();
	instances.clear();

	TestBox newBox =
	{
		++lastActorId,
		{ 0.f, 0.f, 0.f },
		{ 0.f, 0.f, 0.f },
		{ 0.f, 0.f, 0.f },
		{ 0.f, 0.f, 0.f },
		{ 500.f, 200.f + (float)lastActorId * 100.f, 400.f },
		(float)lastActorId * 100.f,
		3.14f / 10.f,
		0.f
	};
	updateBox(newBox, 0.f);

	descriptions.push_back(getBoxDescription(newBox));
	cDescriptions.push_back(descriptions.back().c_str());
	instances.push_back(getBoxInstance(newBox, descriptions.size() - 1));

	TestPlayer newPlayer =
	{
		p_Connection,
		{
			++lastActorId,
			{ 500.f - lastActorId * 200.f, playerSphereRadius, 600.f },
			{ 0.f, 0.f, 0.f },
			{ 0.f, 0.f, 0.f },
			{ 0.f, 0.f, 0.f }
		}
	};

	descriptions.push_back(getPlayerBoxDescription(newPlayer.m_PlayerBox));
	cDescriptions.push_back(descriptions.back().c_str());
	instances.push_back(getBoxInstance(newPlayer.m_PlayerBox, descriptions.size() - 1));

	std::lock_guard<std::mutex> lock(g_ControllerLock);
	players.push_back(newPlayer);

	for(auto& player : players)
	{
		player.m_Connection->sendCreateObjects(cDescriptions.data(), cDescriptions.size(), instances.data(), instances.size());
	}

	boxes.push_back(newBox);

	p_Connection->sendAssignPlayer(newPlayer.m_PlayerBox.actorId);
}

void clientDisconnected(IConnectionController* p_Connection, void* /*p_UserData*/)
{
	Logger::log(Logger::Level::INFO, "Client disconnected");

	std::lock_guard<std::mutex> lock(g_ControllerLock);

	bool playerRemoved = false;
	TestPlayer removedPlayer;

	for(unsigned int i = 0; i < players.size(); i++)
	{
		if(players[i].m_Connection == p_Connection)
		{
			removedPlayer = players[i];
			playerRemoved = true;

			players.erase(players.begin() + i);
			break;
		}
	}

	if (playerRemoved)
	{
		for (auto& player : players)
		{
			player.m_Connection->sendRemoveObjects(&removedPlayer.m_PlayerBox.actorId, 1);
		}
	}
}

void sendTestData()
{
	removeBox = true;
}

void sendPulseObject()
{
	pulseObject = true;
}

const std::string helpMessage =
	"  send     Send data to client\n"
	"  pulse    Pulse an object\n"
	"  list     List all the connected clients\n"
	"  exit     Shutdown the server\n";

void printHelp()
{
	std::cout << helpMessage;
}

void printClientList()
{
	std::lock_guard<std::mutex> lock(g_ControllerLock);

	for (unsigned int i = 0; i < players.size(); i++)
	{
		std::cout << players[i].m_Connection << std::endl;
	}
}

void printUnknownCommand()
{
	std::cout << "Unknown command. Use 'help' for available commands." << std::endl;
}

int main(int argc, char* argv[])
{
	std::ofstream logFile("logFile.txt", std::ofstream::trunc);

	Logger::addOutput(Logger::Level::TRACE, logFile);
	Logger::addOutput(Logger::Level::INFO, std::cout);
	Logger::log(Logger::Level::INFO, "Starting server");

	INetwork* server;
	server = INetwork::createNetwork();
	server->initialize();
	server->createServer(31415);
	server->setClientConnectedCallback(&clientConnected, nullptr);
	server->setClientDisconnectedCallback(&clientDisconnected, nullptr);
	server->startServer(3);

	running = true;
	updateThread = std::thread(updateClients);

	std::string input;
	std::cout << "> ";
	while (true)
	{
		if (!std::getline(std::cin, input))
			break;

		std::transform(input.begin(), input.end(), input.begin(), ::tolower);

		if (input == "exit")
			break;
		else if (input == "send")
			sendTestData();
		else if (input == "help")
			printHelp();
		else if (input == "list")
			printClientList();
		else if (input == "pulse")
			sendPulseObject();
		else
			printUnknownCommand();

		std::cout << "> ";
	}

	running = false;
	updateThread.join();

	INetwork::deleteNetwork(server);
}