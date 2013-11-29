#include <boost/test/unit_test.hpp>
#include "../../../Network/Source/NetworkHandler.h"
#include "../../../Network/Source/MyExceptions.h"

#include <boost/thread.hpp>

#include <condition_variable>

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

std::mutex serverConnect;
std::condition_variable connected;
Result res = Result::FAILURE;
bool done = false;
void actionDone(Result p_Res, void* p_UserData)
{
	std::unique_lock<std::mutex> lock(serverConnect);
	res = p_Res;
	done = true;
	connected.notify_all();
}

BOOST_AUTO_TEST_CASE(TestConnect)
{
	std::shared_ptr<NetworkHandler> server = std::make_shared<NetworkHandler>(31415);
	
	server->startServer();
	boost::thread ioThread(&ioRun, boost::ref(server->getServerService()));

	{
		NetworkHandler client = NetworkHandler();
		client.connectToServer("localhost", 31415, &actionDone, nullptr);

		std::unique_lock<std::mutex> lock(serverConnect);
		while (!done)
			connected.wait(lock);

		BOOST_CHECK(res == Result::SUCCESS);
	}

	server->stopServer();
	ioThread.join();
}

BOOST_AUTO_TEST_SUITE_END()