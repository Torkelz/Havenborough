#include <boost/test/unit_test.hpp>
#include "../../../Network/Source/ClientConnect.h"
#include "../../../Network/Source/MyExceptions.h"
#include "../../../Network/Source/ServerAccept.h"

#include <boost/thread.hpp>

#include <condition_variable>

BOOST_AUTO_TEST_SUITE(TestNetworkServerClient)

std::mutex serverConnect;
std::condition_variable serverConnected;
Result res = Result::FAILURE;
bool done = false;
void actionDone(Result p_Res, void* p_UserData)
{
	std::unique_lock<std::mutex> lock(serverConnect);
	res = p_Res;
	done = true;
	serverConnected.notify_all();
}

std::mutex clientConnect;
std::condition_variable clientConnected;
bool clientConn = false;
void clientConnectedCallback(IConnectionController* p_Connection, void* p_UserData)
{
	std::unique_lock<std::mutex> lock(clientConnect);
	clientConn = true;
	clientConnected.notify_all();
}

std::mutex clientDisconnect;
std::condition_variable clientDisconnected;
bool clientDisconn = false;
void clientDisconnectedCallback(IConnectionController* p_Connection, void* p_UserData)
{
	std::unique_lock<std::mutex> lock(clientDisconnect);
	clientDisconn = true;
	clientDisconnected.notify_all();
}

BOOST_AUTO_TEST_CASE(TestConnect)
{
	boost::asio::io_service ioService;

	std::vector<PackageBase::ptr> prototypes;
	ServerAccept server(ioService, 31415, prototypes);
	server.setConnectedCallback(clientConnectedCallback, nullptr);
	server.setDisconnectedCallback(clientDisconnectedCallback, nullptr);
	server.startServer(3);

	{
		ClientConnect clientConnector(ioService, "localhost", 31415, std::bind(&actionDone, std::placeholders::_1, nullptr));

		std::unique_lock<std::mutex> lock(serverConnect);
		while (!done)
			serverConnected.wait(lock);
		
		std::unique_lock<std::mutex> lock2(clientConnect);
		while (!clientConn)
			clientConnected.wait(lock2);
		
		BOOST_CHECK_EQUAL((int)res, (int)Result::SUCCESS);
		BOOST_CHECK(clientConn);
	}

	std::unique_lock<std::mutex> lock(clientDisconnect);
	while (!clientDisconn)
		clientDisconnected.wait(lock);

	server.stopServer();
}

BOOST_AUTO_TEST_SUITE_END()