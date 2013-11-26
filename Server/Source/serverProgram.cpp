#include "Server.h"

int main(int argc, char* argv[])
{
	boost::asio::io_service service;

	std::shared_ptr< NetworkServer > server = std::make_shared<NetworkServer>(std::ref(service), 31415);
	server->start();

	service.run();
}