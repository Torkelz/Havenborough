#include "BaseGameApp.h"
#include "Logger.h"
#include "ClientExceptions.h"
#include <NetworkExceptions.h>

#include <boost/exception/all.hpp>
#include <boost/system/system_error.hpp>

#include <fstream>
#include <iostream>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main(int /*argc*/, char* /*argv*/[])
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	std::ofstream logFile("logFile.txt", std::ofstream::trunc);

#ifdef _DEBUG
	Logger::addOutput(Logger::Level::DEBUG_L, logFile);
#else
	Logger::addOutput(Logger::Level::INFO, logFile);
#endif

	Logger::addOutput(Logger::Level::INFO, std::cout);
	Logger::log(Logger::Level::INFO, "Starting game");

	try
	{
		BaseGameApp game;
		game.init();
		game.run();
		game.shutdown();

		Logger::log(Logger::Level::INFO, "Closed the game");
	}
	catch (std::exception& err)
	{
		Logger::log(Logger::Level::FATAL, err.what());
		logFile.close();
#ifdef _DEBUG
		throw;
#else
		return EXIT_FAILURE;
#endif
	}
	catch (...)
	{
		Logger::log(Logger::Level::FATAL, "Unknown exception caught, aborting program");
		logFile.close();
#ifdef _DEBUG
		throw;
#else
		return EXIT_FAILURE;
#endif
	}

	logFile.close();
	return EXIT_SUCCESS;
}
