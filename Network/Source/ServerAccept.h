/**
 * File comment.
 */

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

/**
 * Represents the server part of the network.
 *
 * ServerAccept maintains all client connections and
 * is responsible for accepting new ones.
 */
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
	 * Set a callback for when a new client has connected to the server.
	 *
	 * The callback is called before any communication starts with the client.
	 *
	 * @param p_ConnectCallback the function to be called for connected clients.
	 * @param p_Userdata user define data to be passed unmodified to the callback when called.
	 */
	void setConnectedCallback(INetwork::clientConnectedCallback_t p_ConnectCallback, void* p_UserData);

	/**
	 * Set a callback for when a client has disconnected from the server.
	 *
	 * The callback is called before the connection has been removed.
	 *
	 * @param p_ConnectCallback the function to be called for disconnected clients.
	 * @param p_Userdata user define data to be passed unmodified to the callback when called.
	 */
	void setDisconnectedCallback(INetwork::clientDisconnectedCallback_t p_DisconnectCallback, void* p_UserData);

	/**
	* Check if there has occured an error with the acceptor.
	*
	* @return true if there is an error, otherwise false.
	*/
	bool hasError() const;

private:
	void handleAccept(const boost::system::error_code& p_Error);
	void startThreads(unsigned int p_NumThreads);
	void IO_Run();
	void handleDisconnectCallback(ConnectionController* p_Connection);
};