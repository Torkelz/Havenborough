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
	
	std::vector<PackageBase::ptr>& m_PackagePrototypes;
	std::vector<ConnectionController::ptr> m_ConnectedClients;

public:
	/**
	* Used by the server to handle connections.
	* @param p_Port, the port number for the applicatioin to listen to.
	*/
	ServerAccept(boost::asio::io_service& p_IO_Service, unsigned short p_Port, std::vector<PackageBase::ptr>& p_Prototypes);
	/**
	* De-constructor
	*/
	~ServerAccept();
	/*
	* Start the server up.
	*/
	void startServer(INetwork::clientConnectedCallback_t p_ConnectCallback, void* p_UserData, unsigned int p_NumThreads);
	/*
	* Shut down the server.
	*/
	void stopServer();
	/*
	* Get the connection status 
	* @return state::Invalid
	*/
	bool hasError() const;

private:
	void handleAccept(const boost::system::error_code& p_Error);
	void startThreads(unsigned int p_NumThreads);
	void IO_Run();
};