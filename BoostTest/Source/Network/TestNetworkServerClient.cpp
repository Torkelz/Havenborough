#include <boost/test/unit_test.hpp>
#include "../../../Network/Source/ClientConnect.h"
#include "../../../Network/Source/MyExceptions.h"
#include "../../../Network/Source/ServerAccept.h"

#include <boost/thread.hpp>

#include <condition_variable>

BOOST_AUTO_TEST_SUITE(TestNetworkServerClient)

std::mutex responseToClient;
std::condition_variable clientReceivedResponse;
Result clientConnectionResult = Result::FAILURE;
bool clientReceivedResponseCheck = false;
void clientResponse(Result p_Res, void* p_UserData)
{
	std::unique_lock<std::mutex> clientLock(responseToClient);
	clientConnectionResult = p_Res;
	clientReceivedResponseCheck = true;
	clientReceivedResponse.notify_all();
}

std::mutex serverClientConnect;
std::condition_variable serverClientConnected;
bool serverClientConnectedCheck = false;
void serverClientConnectedCallback(IConnectionController* p_Connection, void* p_UserData)
{
	std::unique_lock<std::mutex> serverLock(serverClientConnect);
	serverClientConnectedCheck = true;
	serverClientConnected.notify_all();
}

std::mutex serverClientDisconnect;
std::condition_variable serverClientDisconnected;
bool serverClientDisconnectedCheck = false;
void serverClientDisconnectedCallback(IConnectionController* p_Connection, void* p_UserData)
{
	std::unique_lock<std::mutex> lock(serverClientDisconnect);
	serverClientDisconnectedCheck = true;
	serverClientDisconnected.notify_all();
}

BOOST_AUTO_TEST_CASE(TestConnect)
{
	std::unique_lock<std::mutex> clientLock(responseToClient);
	std::unique_lock<std::mutex> serverLock(serverClientConnect);
	std::unique_lock<std::mutex> serverLock2(serverClientDisconnect);

	boost::asio::io_service ioService;

	std::vector<PackageBase::ptr> prototypes;
	ServerAccept server(ioService, 31415, prototypes);
	server.setConnectedCallback(serverClientConnectedCallback, nullptr);
	server.setDisconnectedCallback(serverClientDisconnectedCallback, nullptr);
	server.startServer(4);

	{
		ClientConnect clientConnector(ioService, "localhost", 31415, std::bind(&clientResponse, std::placeholders::_1, nullptr));

		while (!clientReceivedResponseCheck)
			clientReceivedResponse.wait(clientLock);
		
		while (!serverClientConnectedCheck)
			serverClientConnected.wait(serverLock);
		
		BOOST_REQUIRE_EQUAL((int)clientConnectionResult, (int)Result::SUCCESS);
	}

	while (!serverClientDisconnectedCheck)
		serverClientDisconnected.wait(serverLock2);

	server.stopServer();
}

BOOST_AUTO_TEST_SUITE_END()