#pragma once

#include <CommonTypes.h>
#include <INetwork.h>

#include "Connection.h"
#include "ConnectionController.h"
#include "Packages.h"

#include <atomic>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <mutex>

class ServerAccept
{
private:
	unsigned short m_PortNumber;

	boost::asio::io_service& m_IO_Service;
	boost::asio::ip::tcp::acceptor m_Acceptor;
	boost::asio::ip::tcp::socket m_AcceptSocket;

	std::vector<boost::thread> m_WorkerThreads;

	bool m_HasError;
	bool m_Running;

	INetwork::clientConnectedCallback_t m_ClientConnected;
	void* m_ClientConnectedUserData;
	INetwork::clientDisconnectedCallback_t m_ClientDisconnected;
	void* m_ClientDisconnectedUserData;
	
	std::vector<PackageBase::ptr>& m_PackagePrototypes;
	std::vector<ConnectionController::ptr> m_ConnectedClients;

public:
	/**
	 * Constructor.
	 *
	 * Creates an empty object to later be started.
	 *
	 * @param p_IO_Service the io service to be used by the object for asynchronous actions.
	 *			The ServerAccept object will manage any worker threads needed for its tasks.
	 * @param p_Port the port number for the applicatioin to listen to.
	 * @param p_Prototype a list containing prototypes for any
	 *			packagaes supported by the servers protocol.
	 */
	ServerAccept(boost::asio::io_service& p_IO_Service, unsigned short p_Port, std::vector<PackageBase::ptr>& p_Prototypes);

	/**
	 * De-constructor.
	 *
	 * If the server is running it will be stopped and its worker threads stopped.
	 * Any connected clients are disconnected.
	 */
	~ServerAccept();

	/**
	 * Start the server.
	 *
	 * Starts accepting clients. For each client, a connection
	 * is established and communication is started.
	 * Worker threads are started to handle the communication.
	 *
	 * @param p_NumThreads the number of worker threads to service client connections.
	 */
	void startServer(unsigned int p_NumThreads);

	/**
	 * Shut down the server.
	 *
	 * Stops the io service and all worker threads. All connected clients are disconnected.
	 */
	void stopServer();

	/**
	 * Callback function to the server that there is a connection to a client.
	 * @param p_ConnectCallback is the return function.
	 * @param p_Userdata user define data.
	 */
	void setConnectedCallback(INetwork::clientConnectedCallback_t p_ConnectCallback, void* p_UserData);
	/**
	* Callback function to the server that the connection to a client broke.
	* @param p_ConnectCallback is the return function.
	* @param p_Userdata user defined data.
	*/
	void setDisconnectedCallback(INetwork::clientDisconnectedCallback_t p_DisconnectCallback, void* p_UserData);

	/**
	* Get the connection status 
	* @return state::Invalid
	*/
	bool hasError() const;

private:
	void handleAccept(const boost::system::error_code& p_Error);
	void startThreads(unsigned int p_NumThreads);
	void IO_Run();
	void handleDisconnectCallback(ConnectionController* p_Connection);
};