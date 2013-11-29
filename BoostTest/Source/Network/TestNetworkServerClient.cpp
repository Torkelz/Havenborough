#include <boost/test/unit_test.hpp>
#include "../../../Client/Source/NetworkClient.h"
#include "../../../Server/Source/NetworkServer.h"

#include <boost/thread.hpp>

BOOST_AUTO_TEST_SUITE(TestNetworkServerClient)

void ioRun(boost::asio::io_service& p_IO_Service)
{
	p_IO_Service.run();
}

BOOST_AUTO_TEST_CASE(TestConnect)
{
	boost::asio::io_service ioService;
	std::shared_ptr<NetworkServer> server = std::make_shared<NetworkServer>(ioService, 31415);
	
	server->start();
	
	boost::thread ioThread(&ioRun, boost::ref(ioService));

	{
		NetworkClient client;
		client.connect("localhost");

		while (!client.isConnected() && !client.hasError())
		{}

		BOOST_CHECK(client.isConnected());
	}

	server->stop();
	ioThread.join();
}

BOOST_AUTO_TEST_SUITE_END()