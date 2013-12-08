#include "ModelConverter.h"
#include <iostream>

int main(int argc, char* argv[])
{
	ModelConverter loader;
	loader.loadFile("Character_Witch.tx");
	loader.writeFile("Character_Witch.tx.btx");
	loader.loadBinaryFile("Character_Witch.tx.btx");
	system("PAUSE");
}

