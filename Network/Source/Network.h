#pragma once

#include "MyExceptions.h"
#include "../include/INetwork.h"
#include "NetworkHandler.h"
#include "Packages.h"

class Network : public INetwork
{
private:
	NetworkHandler* m_Handler;
	boost::thread m_IOThread;
public:
	Network();
	~Network();

	void createServer(unsigned short p_Port);
	void createClient(unsigned short p_Port);
	void turnOfServer();

	bool isConnected() const override;
	bool hasError() const;
	void connectToServer(const std::string& p_URL, unsigned short p_Port);
	void sendAddObject(float,float,float);

	boost::asio::io_service& getServerService();

};