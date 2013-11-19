#include "MyExceptions.h"
#include "Window.h"

#include <iostream>

int main(int argc, char* argv[])
{
	try
	{
		std::cout << "Hello World!" << std::endl;

		UVec2 size = {800, 480};
		Window myWindow;
		myWindow.init("My awesome window title!", size);
		while (true)
		{
			myWindow.pollMessages();
		}
		myWindow.destroy();

		return EXIT_SUCCESS;
	}
	catch (MyException& err)
	{
		std::cerr << err.what() << std::endl;
		return EXIT_FAILURE;
	}
}
