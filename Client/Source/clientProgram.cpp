#include "MyExceptions.h"
#include "BaseGameApp.h"

#include <iostream>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	BaseGameApp game;

	try
	{
		game.init();
		game.run();
		game.shutdown();
		
		return EXIT_SUCCESS;
	}
	catch (MyException& err)
	{
		std::cerr << err.what() << std::endl;
		return EXIT_FAILURE;
	}
}
