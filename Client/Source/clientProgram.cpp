#include "BaseGameApp.h"
#include "Logger.h"
#include "ClientExceptions.h"
#include <NetworkExceptions.h>

#include <boost/exception/all.hpp>
#include <boost/system/system_error.hpp>

#include <fstream>
#include <iostream>

#define USE_VLD

#ifdef USE_VLD
#include <vld.h>

#define SET_DBG_FLAG
#define DUMP_LEAKS
#else
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define SET_DBG_FLAG _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF )
#define DUMP_LEAKS _CrtDumpMemoryLeaks()
#endif

#ifdef _DEBUG
#define DEFAULT_LOG_LEVEL DEBUG_L
#define HANDLE_EXCEPTION throw
#else
#define DEFAULT_LOG_LEVEL INFO
#define HANDLE_EXCEPTION return EXIT_FAILURE
#endif

int main(int /*argc*/, char* /*argv*/[])
{
	SET_DBG_FLAG;

	{
		std::ofstream logFile("logFile.txt", std::ofstream::trunc);

		Logger::addOutput(Logger::Level::DEFAULT_LOG_LEVEL, logFile);

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
			HANDLE_EXCEPTION;
		}
		catch (...)
		{
			Logger::log(Logger::Level::FATAL, "Unknown exception caught, aborting program");
			logFile.close();
			HANDLE_EXCEPTION;
		}

		logFile.close();
	}

	DUMP_LEAKS;
	return EXIT_SUCCESS;
}
