#include "INetwork.h"
#include <iostream>

IConnectionController* g_Controller;

void clientConnected(IConnectionController* p_Connection, void* p_UserData)
{
	std::cout << "Client connected" << std::endl;
	g_Controller = p_Connection;
	AddObjectData data = {5.f, 4.f, 1.f};
	g_Controller->sendAddObject(data);
	g_Controller->sendAddObject(data);
}

int main(int argc, char* argv[])
{
	INetwork* server;
	server = INetwork::createNetwork();
	char test;
	server->createServer(31415, &clientConnected, nullptr, 3);

	std::cin >> test;

	INetwork::deleteNetwork(server);
}