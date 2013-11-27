#include "INetwork.h"

int main(int argc, char* argv[])
{
	INetwork* server;
	server = INetwork::createNetwork();

	server->createServer(31415);
	char test;
	std::cin >> test;

	INetwork::deleteNetwork(server);
}