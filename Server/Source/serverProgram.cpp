#include <Logger.h>
#include <TweakSettings.h>

#include "Server.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

Server server;

void printHelp()
{
	static const std::string helpMessage =
		"  help     Print this message\n"
		"  send     Send data to client\n"
		"  pulse    Pulse an object\n"
		"  list     List all the connected clients\n"
		"  games    List all running games\n"
		"  exit     Shutdown the server\n";

	std::cout << helpMessage;
}

void listUsers()
{
	for (const auto& user : server.getUserNames())
	{
		std::cout << user << std::endl;
	}
}

void listGames()
{
	for (const auto& game : server.getGameDescriptions())
	{
		std::cout << game << std::endl;
	}
}

void printUnknownCommand()
{
	std::cout << "Unknown command. Use 'help' for available commands." << std::endl;
}

int main(int argc, char* argv[])
{
	std::ofstream logFile("serverLogFile.txt", std::ofstream::trunc);

	Logger::addOutput(Logger::Level::TRACE, logFile);
	Logger::addOutput(Logger::Level::INFO, std::cout);
	Logger::log(Logger::Level::INFO, "Starting server");

	TweakSettings::initializeMaster();

	server.initialize();
	server.run();

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
			server.sendTestData();
		else if (input == "help")
			printHelp();
		else if (input == "list")
			listUsers();
		else if (input == "games")
			listGames();
		else if (input == "pulse")
			server.sendPulseObject();
		else
			printUnknownCommand();

		std::cout << "> ";
	}

	server.shutdown();

	TweakSettings::shutdown();
}