#include <INetwork.h>
#include "../../Client/Source/Logger.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<IConnectionController*> g_Controllers;

void clientConnected(IConnectionController* p_Connection, void* p_UserData)
{
	Logger::log(Logger::Level::INFO, "Client connected");

	AddObjectData data = {5.f, 4.f, 1.f};
	p_Connection->sendAddObject(data);
	p_Connection->sendAddObject(data);

	for (auto& con : g_Controllers)
	{
		con->sendAddObject(data);
	}

	g_Controllers.push_back(p_Connection);
}

void clientDisconnected(IConnectionController* p_Connection, void* p_UserData)
{
	Logger::log(Logger::Level::INFO, "Client disconnected");

	for(unsigned int i = 0; i < g_Controllers.size(); i++)
	{
		if(g_Controllers[i] == p_Connection)
		{
			g_Controllers.erase(g_Controllers.begin() + i);
			break;
		}
	}
	AddObjectData data = {-5.f, -4.f, -1.f};
	for(auto& con : g_Controllers)
	{
		con->sendAddObject(data);
	}
}

void sendTestData()
{
	AddObjectData data = {3.f, 4.f, 1.f};

	for (auto& con : g_Controllers)
	{
		con->sendAddObject(data);
	}
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

	INetwork::deleteNetwork(server);
}