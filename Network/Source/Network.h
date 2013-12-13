/**
 * File comment.
 */

#pragma once

#include "ClientConnect.h"
#include "ConnectionController.h"
#include "NetworkExceptions.h"
#include "Packages.h"
#include "ServerAccept.h"
#include "../include/INetwork.h"

/**
 * Implementation of the INetwork interface.
 */
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
	/**
	 * constructor.
	 */
	Network();
	/**
	 * destructor.
	 */
	~Network();

	void initialize() override;

	void createServer(unsigned short p_Port) override;
	void startServer(unsigned int p_NumThreads) override;

	void setClientConnectedCallback(clientConnectedCallback_t p_ConnectCallback, void* p_UserData) override;
	void setClientDisconnectedCallback(clientDisconnectedCallback_t p_DisconnectCallback, void* p_UserData) override;

	void turnOffServer() override;
	void connectToServer(const char* p_URL, unsigned short p_Port, actionDoneCallback p_DoneHandler, void* p_UserData) override;

	IConnectionController* getConnectionToServer() override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;

private:
	void registerPackages();

	void startIO();
	void IO_Run();
	void clientConnectionDone(Result p_Result, actionDoneCallback p_DoneHandler, void* p_UserData);
	void clientDisconnected(actionDoneCallback p_DoneHandler, void* p_UserData);
};
