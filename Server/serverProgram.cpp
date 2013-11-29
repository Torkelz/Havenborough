#include "INetwork.h"
#include <iostream>

int main(int argc, char* argv[])
{
	INetwork* server;
	server = INetwork::createNetwork();
	char test;
	server->createServer(31415);
	std::cin >> test;
	AddObjectData data = {5.f, 4.f, 1.f};
	//server->sendAddObject(data);
	//server->sendAddObject(data);

	std::cin >> test;

	INetwork::deleteNetwork(server);
}