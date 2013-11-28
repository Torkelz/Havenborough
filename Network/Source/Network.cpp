#include "Network.h"

#include <boost/archive/binary_oarchive.hpp>

Network::Network()
{
	m_Handler = NULL;


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
	m_Handler->startServer();
}

void Network::turnOfServer()
{
	m_Handler->stopServer();
}

void Network::createClient(unsigned short p_Port)
{
	m_Handler = new NetworkHandler();
}

void Network::connectToServer(const std::string& p_URL, unsigned short p_Port)
{
	m_Handler->connectToServer(p_URL, p_Port);
}

bool Network::isConnected() const
{
	return m_Handler->isConnected();
}

bool Network::hasError() const
{
	return m_Handler->hasError();
}

boost::asio::io_service& Network::getServerService()
{
	return m_Handler->getServerService();
}

void Network::sendAddObject(float, float, float)
{

}