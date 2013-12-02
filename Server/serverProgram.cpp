#include "INetwork.h"

#include <iostream>
#include <vector>

std::vector<IConnectionController*> g_Controllers;

void clientConnected(IConnectionController* p_Connection, void* p_UserData)
{
	std::cout << "Client connected" << std::endl;
	AddObjectData data = {5.f, 4.f, 1.f};
	p_Connection->sendAddObject(data);
	p_Connection->sendAddObject(data);

	for (auto& con : g_Controllers)
	{
		con->sendAddObject(data);
	}

	g_Controllers.push_back(p_Connection);
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