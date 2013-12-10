#include <boost/test/unit_test.hpp>
#include "../../../Network/include/NetworkExceptions.h"
#include "../../../Network/Source/ClientConnect.h"
#include "../../../Network/Source/Network.h"
#include "../../../Network/Source/ServerAccept.h"

#include <condition_variable>

BOOST_AUTO_TEST_SUITE(TestNetworkServerClient)

const unsigned short testPort = 12345;

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
	ServerAccept server(ioService, testPort, prototypes);
	server.setConnectedCallback(serverClientConnectedCallback, nullptr);
	server.setDisconnectedCallback(serverClientDisconnectedCallback, nullptr);
	server.startServer(4);

	{
		ClientConnect clientConnector(ioService, "localhost", testPort, std::bind(&clientResponse, std::placeholders::_1, nullptr));

		while (!clientReceivedResponseCheck)
			clientReceivedResponse.wait(clientLock);
		
		BOOST_REQUIRE_EQUAL((int)clientConnectionResult, (int)Result::SUCCESS);
		
		while (!serverClientConnectedCheck)
			serverClientConnected.wait(serverLock);
	}

	while (!serverClientDisconnectedCheck)
		serverClientDisconnected.wait(serverLock2);

	server.stopServer();
}

///// Should probably be an integration test instead. ///////////

std::mutex responseToClient2;
std::condition_variable clientReceivedResponse2;
Result clientConnectionResult2 = Result::FAILURE;
bool clientReceivedResponseCheck2 = false;
void clientResponse2(Result p_Res, void* p_UserData)
{
	std::unique_lock<std::mutex> clientLock(responseToClient2);
	clientConnectionResult2 = p_Res;
	clientReceivedResponseCheck2 = true;
	clientReceivedResponse2.notify_all();
}

std::mutex serverClientConnect2;
std::condition_variable serverClientConnected2;
bool serverClientConnectedCheck2 = false;
void serverClientConnectedCallback2(IConnectionController* p_Connection, void* p_UserData)
{
	std::unique_lock<std::mutex> serverLock(serverClientConnect2);
	serverClientConnectedCheck2 = true;
	serverClientConnected2.notify_all();
}

std::mutex serverClientDisconnect2;
std::condition_variable serverClientDisconnected2;
bool serverClientDisconnectedCheck2 = false;
void serverClientDisconnectedCallback2(IConnectionController* p_Connection, void* p_UserData)
{
	std::unique_lock<std::mutex> lock(serverClientDisconnect2);
	serverClientDisconnectedCheck2 = true;
	serverClientDisconnected2.notify_all();
}

BOOST_AUTO_TEST_CASE(TestNetwork)
{
	std::unique_lock<std::mutex> clientLock(responseToClient);
	std::unique_lock<std::mutex> serverLock(serverClientConnect);
	std::unique_lock<std::mutex> serverLock2(serverClientDisconnect);

	Network serverNetwork;

	serverNetwork.createServer(testPort);
	serverNetwork.setClientConnectedCallback(serverClientConnectedCallback2, nullptr);
	serverNetwork.setClientDisconnectedCallback(serverClientDisconnectedCallback2, nullptr);
	serverNetwork.startServer(4);

	{
		Network clientNetwork;

		clientNetwork.connectToServer("localhost", testPort, clientResponse2, nullptr);

		while (!clientReceivedResponseCheck2)
			clientReceivedResponse2.wait(clientLock);
		
		BOOST_REQUIRE_EQUAL((int)clientConnectionResult2, (int)Result::SUCCESS);
		
		while (!serverClientConnectedCheck2)
			serverClientConnected2.wait(serverLock);

		boost::this_thread::sleep_for(boost::chrono::milliseconds(200));
	}

	while (!serverClientDisconnectedCheck2)
		serverClientDisconnected2.wait(serverLock2);

	serverNetwork.turnOffServer();
}

BOOST_AUTO_TEST_SUITE_END()