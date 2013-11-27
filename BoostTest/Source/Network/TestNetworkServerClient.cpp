#include <boost/test/unit_test.hpp>
#include "../../../Network/Source/NetworkHandler.h"
#include "../../../Network/Source/MyExceptions.h"

#include <boost/thread.hpp>

BOOST_AUTO_TEST_SUITE(TestNetworkServerClient)

void ioRun(boost::asio::io_service& p_IO_Service)
{
	try
	{
		p_IO_Service.run();
	}
	catch (NetworkError&)
	{

	}
}

BOOST_AUTO_TEST_CASE(TestConnect)
{
	std::shared_ptr<NetworkHandler> server = std::make_shared<NetworkHandler>(31415);
	
	server->startServer();
	boost::thread ioThread(&ioRun, boost::ref(server->getServerService()));

	{
		NetworkHandler client = NetworkHandler();
		client.connectToServer("localhost");

		while (!client.isConnected() && !client.hasError())
		{}

		BOOST_CHECK(client.isConnected());
	}
//
	server->stopServer();
	ioThread.join();
}

BOOST_AUTO_TEST_SUITE_END()