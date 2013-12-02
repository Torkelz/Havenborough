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

	void createServer(unsigned short p_Port, clientConnectedCallback_t p_ConnectCallback, void* p_UserData, unsigned int p_NumThreads) override;
	void turnOfServer() override;

	void connectToServer(const char* p_URL, unsigned short p_Port, actionDoneCallback p_DoneHandler, void* p_UserData) override;
	IConnectionController* getConnectionToServer() override;

private:
	void registerPackages();

	void startIO();
	void IO_Run();
	void clientConnectionDone(Result p_Result, actionDoneCallback p_DoneHandler, void* p_UserData);
};