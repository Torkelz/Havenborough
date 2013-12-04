#include "MyExceptions.h"
#include "BaseGameApp.h"

#include <iostream>

int main(int argc, char* argv[])
{
	BaseGameApp game;

	try
	{
		game.init(argv[0]);
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
