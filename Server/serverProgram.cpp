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
std::vector<IConnectionController*> g_Controllers;

const float boxCircleCenter[3] = {5.f, 4.f, 6.f};
const float boxCircleRadius = 5.f;
const float boxCircleRotationSpeed = 3.141592f / 10.f;
float boxCircleRotation = 0.f;

bool running = false;
std::thread updateThread;

void updateClients()
{
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previousTime;
	float deltaTime = 0.001f;

	while (running)
	{
		boxCircleRotation += boxCircleRotationSpeed * deltaTime;

		UpdateObjectData data =
		{
			{
				boxCircleCenter[0] + cos(boxCircleRotation) * boxCircleRadius,
				boxCircleCenter[1],
				boxCircleCenter[2] - sin(boxCircleRotation) * boxCircleRadius
			},
			{
				-sin(boxCircleRotation) * boxCircleRadius * boxCircleRotationSpeed,
				0.f,
				-cos(boxCircleRotation) * boxCircleRadius * boxCircleRotationSpeed
			},
			{
				boxCircleRotation,
				0.f,
				boxCircleRotation
			},
			{
				boxCircleRotationSpeed,
				0.f,
				0.f
			},
			1
		};

		std::lock_guard<std::mutex> lock(g_ControllerLock);
		for (auto& con : g_Controllers)
		{
			con->sendUpdateObjects(&data, 1, nullptr, 0);
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

	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Movement");
	printer.OpenElement("Velocity");
	printer.PushAttribute("x", 10.f);
	printer.PushAttribute("y", 0.f);
	printer.PushAttribute("z", 0.f);
	printer.CloseElement();
	printer.OpenElement("RotationalVelocity");
	printer.PushAttribute("x", boxCircleRotationSpeed);
	printer.PushAttribute("y", 0.f);
	printer.PushAttribute("z", 0.f);
	printer.CloseElement();
	printer.CloseElement();
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "BOX");
	printer.CloseElement();
	printer.OpenElement("OBBPhysics");
	printer.OpenElement("Halfsize");
	printer.PushAttribute("x", 0.5f);
	printer.PushAttribute("y", 0.5f);
	printer.PushAttribute("z", 0.5f);
	printer.CloseElement();
	printer.CloseElement();
	printer.CloseElement();
	const char* desc = printer.CStr();
	ObjectInstance inst = {{5.f, 4.f, 1.f}, {1.f, 1.f, 1.f}, 0, 1};

	p_Connection->sendCreateObjects(&desc, 1, &inst, 1);

	std::lock_guard<std::mutex> lock(g_ControllerLock);
	g_Controllers.push_back(p_Connection);
}

void clientDisconnected(IConnectionController* p_Connection, void* /*p_UserData*/)
{
	Logger::log(Logger::Level::INFO, "Client disconnected");

	std::lock_guard<std::mutex> lock(g_ControllerLock);

	for(unsigned int i = 0; i < g_Controllers.size(); i++)
	{
		if(g_Controllers[i] == p_Connection)
		{
			g_Controllers.erase(g_Controllers.begin() + i);
			break;
		}
	}

	//const char* desc = "Model:DZALA\n";
	//ObjectInstance inst = {{-5.f, 4.f, -1.f}, {0.f, 0.f, 0.f}, 0, 1};

	//for(auto& con : g_Controllers)
	//{
	//	con->sendCreateObjects(&desc, 1, &inst, 1);
	//}
}

void sendTestData()
{
	//const char* desc = "Model:BOX\nCollision:OBB 0 0 0 0.5 0.5 0.5 0 0 0\n";
	//ObjectInstance inst = {{5.f, 2.f, 1.f}, {0.f, 0.f, 0.f}, 0, 1};

	//std::lock_guard<std::mutex> lock(g_ControllerLock);
	//for(auto& con : g_Controllers)
	//{
	//	inst.m_Position[0] -= 2.f;
	//	con->sendCreateObjects(&desc, 1, &inst, 1);
	//}
}

const std::string helpMessage =
	"  send     Send data to client\n"
	"  list     List all the connected clients\n"
	"  exit     Shutdown the server\n";

void printHelp()
{
	std::cout << helpMessage;
}

void printClientList()
{
	std::lock_guard<std::mutex> lock(g_ControllerLock);

	for (unsigned int i = 0; i < g_Controllers.size(); i++)
	{
		std::cout << g_Controllers[i] << std::endl;
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
		else
			printUnknownCommand();

		std::cout << "> ";
	}

	running = false;
	updateThread.join();

	INetwork::deleteNetwork(server);
}