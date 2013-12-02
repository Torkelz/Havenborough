#include "Network.h"

Network::Network()
	:	m_IO_Started(false)
{
	registerPackages();
}

Network::~Network()
{
	m_IO_Service.stop();

	if (m_IO_Thread.joinable())
	{
		m_IO_Thread.join();
	}
}

INetwork *INetwork::createNetwork()
{
	return new Network();
}

void INetwork::deleteNetwork(INetwork * p_Network)
{
	delete p_Network;
}

void Network::createServer(unsigned short p_Port, clientConnectedCallback_t p_ConnectCallback, void* p_UserData, unsigned int p_NumThreads)
{
	m_ServerAcceptor.reset();
	m_ServerAcceptor.reset(new ServerAccept(m_IO_Service, p_Port, m_PackagePrototypes));
	m_ServerAcceptor->startServer(p_ConnectCallback, p_UserData, p_NumThreads);
}

void Network::turnOfServer()
{
	m_ServerAcceptor->stopServer();
}

void Network::connectToServer(const char* p_URL, unsigned short p_Port, actionDoneCallback p_DoneHandler, void* p_UserData)
{
	m_ClientConnection.reset();
	m_ClientConnect.reset();
	m_IO_Service.reset();

	if (m_IO_Thread.joinable())
	{
		m_IO_Thread.join();
	}

	m_ClientConnect.reset(new ClientConnect(m_IO_Service, p_URL, p_Port, std::bind(&Network::clientConnectionDone, this, std::placeholders::_1, p_DoneHandler, p_UserData)));

	startIO();
}

IConnectionController* Network::getConnectionToServer()
{
	return m_ClientConnection.get();
}

void Network::registerPackages()
{
	m_PackagePrototypes.push_back(PackageBase::ptr(new AddObject));
}

void Network::startIO()
{
	m_IO_Started = true;
	m_IO_Thread.swap(boost::thread(std::bind(&Network::IO_Run, this)));
}

void Network::IO_Run()
{
	try
	{
		m_IO_Service.run();
	}
	catch (NetworkError& err)
	{
		std::cout << err.what() << std::endl;
	}
	catch (...)
	{
		int dummy = 42;
	}
}

void Network::clientConnectionDone(Result p_Result, actionDoneCallback p_DoneHandler, void* p_UserData)
{
	m_ClientConnection.reset(new ConnectionController(std::unique_ptr<Connection>(new Connection(m_ClientConnect->releaseConnectedSocket())), m_PackagePrototypes));

	if (p_DoneHandler)
	{
		p_DoneHandler(p_Result, p_UserData);
	}

	m_ClientConnection->startListening();
	m_ClientConnect.reset();
}
