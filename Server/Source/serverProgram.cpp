#include "Server.h"

int main(int argc, char* argv[])
{
	boost::asio::io_service service;

	boost::shared_ptr< tcp_server > server = boost::make_shared<tcp_server>(boost::ref(service),33333);
	server->start();

	service.run();
}