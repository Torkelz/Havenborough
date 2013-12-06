#include "ModelLoader.h"
#include <iostream>

int main(int argc, char* argv[])
{
	ModelLoader loader;
	loader.loadFile("../Sample150.tx");
	loader.writeFile("../Sample150.bin");
	system("PAUSE");
}