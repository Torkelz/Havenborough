#pragma once

#include "ClientConnect.h"
#include "ConnectionController.h"
#include "MyExceptions.h"
#include "Packages.h"
#include "ServerAccept.h"
#include "../include/INetwork.h"

class Network : public INetwork
{
private:
	boost::asio::io_service m_IO_Service;
	boost::thread m_IO_Thread;
	bool m_IO_Started;

	std::vector<PackageBase::ptr> m_PackagePrototypes;

	std::unique_ptr<ServerAccept> m_ServerAcceptor;
	std::unique_ptr<ClientConnect> m_ClientConnect;

	ConnectionController::ptr m_ClientConnection;

public:
	Network();
	~Network();

	/**
	* Create the server at the specified port.
	*
	* @param p_Port, port number must be over 1024.
	*/
	void createServer(unsigned short p_Port) override;
 
	/** 
	* Start the server and define the number of threads.
	* 
	* @param p_NumThreads The number of worker threads to spawn. Must be at least 1.
	*/
	void startServer(unsigned int p_NumThreads) override;

	/**
	* 
	*
	* @param p_ConnectCallback The callback to handle connected clients. Null to disable callback.
	* @param p_UserData User defined data to be passed to the callback.
	*/
	void setClientConnectedCallback(clientConnectedCallback_t p_ConnectCallback, void* p_UserData) override;

	/**
	* 
	*
	* @param p_DisconnectCallback The callback to handle connected clients. Null to disable callback.
	* @param p_UserData User defined data to be passed to the callback.
	*/
	void setClientDisconnectedCallback(clientDisconnectedCallback_t p_DisconnectCallback, void* p_UserData) override;

	/*
	* Turn of the server.
	*/
	void turnOfServer() override;

	/*
	* Connect the client to the server.
	*
	* @param p_URL, address to the server.
	* @param p_Port Port number on the server to connect to.
	*/
	void connectToServer(const char* p_URL, unsigned short p_Port, actionDoneCallback p_DoneHandler, void* p_UserData) override;

	/**
	* Returns the Connection from the server.
	*
	* @returns a Connection pointer.
	*/
	IConnectionController* getConnectionToServer() override;

private:
	void registerPackages();

	void startIO();
	void IO_Run();
	void clientConnectionDone(Result p_Result, actionDoneCallback p_DoneHandler, void* p_UserData);
};