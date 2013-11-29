#pragma once

#include "ConnectionController.h"
#include "MyExceptions.h"
#include "../include/INetwork.h"
#include "NetworkHandler.h"
#include "Packages.h"

class Network : public INetwork
{
private:
	NetworkHandler* m_Handler;
	boost::thread m_IOThread;
	std::vector<PackageBase::ptr> m_PackagePrototypes;

	ConnectionController::ptr m_ClientConnection;

public:
	Network();
	~Network();

	void createServer(unsigned short p_Port) override;
	void createClient(unsigned short p_Port) override;
	void turnOfServer() override;

	void connectToServer(const char* p_URL, actionDoneCallback p_DoneHandler, void* p_UserData) override;
	IConnectionController* getConnectionToServer() override;
	
	boost::asio::io_service& getServerService();

private:
	void registerPackages();
};