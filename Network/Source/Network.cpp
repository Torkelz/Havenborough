#include "Network.h"

Network::Network()
{
	m_Handler = NULL;

	registerPackages();
}

Network::~Network()
{

}

INetwork *INetwork::createNetwork()
{
	return new Network();
}

void INetwork::deleteNetwork(INetwork * p_Network)
{
	delete p_Network;
}

void Network::createServer(unsigned short p_Port)
{
	m_Handler = new NetworkHandler(p_Port);
	//m_Handler->setSaveData(std::bind(&Network::savePackageCallBack, this, std::placeholders::_1, std::placeholders::_2));
	m_Handler->startServer();
}

void Network::turnOfServer()
{
	m_Handler->stopServer();
}

void Network::createClient(unsigned short p_Port)
{
	m_Handler = new NetworkHandler(p_Port);
	//m_Handler->setSaveData(std::bind(&Network::savePackageCallBack, this, std::placeholders::_1, std::placeholders::_2));
}

void Network::connectToServer(const char* p_URL, actionDoneCallback p_DoneHandler, void* p_UserData)
{
	m_Handler->connectToServer(p_URL, p_DoneHandler, p_UserData);
}

IConnectionController* Network::getConnectionToServer()
{
	if (!m_ClientConnection)
	{
		m_ClientConnection.reset(new ConnectionController(m_Handler->getClientConnection(), m_PackagePrototypes));
	}

	return m_ClientConnection.get();
}

boost::asio::io_service& Network::getServerService()
{
	return m_Handler->getServerService();
}

void Network::registerPackages()
{
	m_PackagePrototypes.push_back(PackageBase::ptr(new AddObject));
}
