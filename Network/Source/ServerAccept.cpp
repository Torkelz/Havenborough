#include "ServerAccept.h"

#include "ConnectionController.h"
#include "NetworkExceptions.h"
#include "NetworkLogger.h"

ServerAccept::ServerAccept(boost::asio::io_service& p_IO_Service, unsigned short p_Port, std::vector<PackageBase::ptr>& p_Prototypes) 
		:	m_Acceptor(m_IO_Service, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), p_Port)),
			m_AcceptSocket(m_IO_Service),
			m_HasError(false),
			m_Running(false),
			m_PortNumber(p_Port),
			m_PackagePrototypes(p_Prototypes),
			m_IO_Service(p_IO_Service),
			m_ClientConnected(nullptr),
			m_ClientDisconnected(nullptr)
{
	NetworkLogger::log(NetworkLogger::Level::DEBUG, "Creating server acceptor");
}

ServerAccept::~ServerAccept()
{
	NetworkLogger::log(NetworkLogger::Level::DEBUG, "Destroying server acceptor");

	if (m_Running)
	{
		stopServer();
	}
}

void ServerAccept::startServer(unsigned int p_NumThreads)
{
	NetworkLogger::log(NetworkLogger::Level::DEBUG, "Starting server acceptor");

	try
	{
		m_Acceptor.async_accept(m_AcceptSocket, std::bind( &ServerAccept::handleAccept, this, std::placeholders::_1));
		startThreads(p_NumThreads);
	}
	catch (boost::system::system_error& err)
	{
		NetworkLogger::log(NetworkLogger::Level::FATAL, err.what());
	}
}

void ServerAccept::stopServer()
{
	NetworkLogger::log(NetworkLogger::Level::DEBUG, "Stopping server acceptor");

	{
		std::unique_lock<std::mutex> lock(m_ClientLock);
		for (auto& client : m_ConnectedClients)
		{
			client.reset();
		}
		m_ConnectedClients.clear();
	}

	m_Running = false;
	m_IO_Service.stop();

	for (auto& thread : m_WorkerThreads)
	{
		thread.join();
	}
	m_WorkerThreads.clear();
}

void ServerAccept::setConnectedCallback(INetwork::clientConnectedCallback_t p_ConnectCallback, void* p_UserData)
{
	m_ClientConnected = p_ConnectCallback;
	m_ClientConnectedUserData = p_UserData;
}

void ServerAccept::setDisconnectedCallback(INetwork::clientDisconnectedCallback_t p_DisconnectCallback, void* p_UserData)
{
	m_ClientDisconnected = p_DisconnectCallback;
	m_ClientDisconnectedUserData = p_UserData;
}

bool ServerAccept::hasError() const
{
	return m_HasError;
}

void ServerAccept::handleAccept( const boost::system::error_code& error)
{
	NetworkLogger::log(NetworkLogger::Level::TRACE, "Server handling accept");

	if ( error )
	{  
		NetworkLogger::log(NetworkLogger::Level::FATAL, error.message().c_str());
		return;
	}

    Connection::ptr connection(new Connection(std::move(m_AcceptSocket)));
	ConnectionController::ptr clientConnection(new ConnectionController(std::move(connection), m_PackagePrototypes));

	clientConnection->setDisconnectedCallback(std::bind(&ServerAccept::handleDisconnectCallback, this, clientConnection.get()));

	if (m_ClientConnected)
	{
		m_ClientConnected(clientConnection.get(), m_ClientConnectedUserData);
	}

	clientConnection->startListening();

	{
		std::unique_lock<std::mutex> lock(m_ClientLock);
		m_ConnectedClients.push_back(std::move(clientConnection));
	}

	m_Acceptor.async_accept(m_AcceptSocket, std::bind( &ServerAccept::handleAccept, this, std::placeholders::_1));
}

void ServerAccept::startThreads(unsigned int p_NumThreads)
{
	if (p_NumThreads < 1)
		p_NumThreads = 1;

	m_Running = true;

	for (unsigned int i = 0; i < p_NumThreads; i++)
	{
		NetworkLogger::log(NetworkLogger::Level::DEBUG, "Starting server network IO thread");

		m_WorkerThreads.emplace_back(std::bind(&ServerAccept::IO_Run, this));
	}
}

void ServerAccept::IO_Run()
{
	try
	{
		while (m_Running)
		{
			try
			{
				m_IO_Service.run();
			}
			catch (ClientDisconnected& err)
			{
				NetworkLogger::log(NetworkLogger::Level::INFO, "Client disconnected");
				NetworkLogger::log(NetworkLogger::Level::TRACE, err.what());
			}
			catch (NetworkError& err)
			{
				NetworkLogger::log(NetworkLogger::Level::WARNING, err.what());
			}
		}
	}
	catch (...)
	{
		NetworkLogger::log(NetworkLogger::Level::FATAL, "Unknown exception on network IO thread");
	}
	
	NetworkLogger::log(NetworkLogger::Level::DEBUG, "Server IO thread done");
}

void ServerAccept::handleDisconnectCallback(ConnectionController* p_Connection)
{
	if (m_ClientDisconnected)
	{
		m_ClientDisconnected(p_Connection, m_ClientDisconnectedUserData);
	}

	m_IO_Service.post(std::bind(&ServerAccept::removeClient, this, p_Connection));
}

void ServerAccept::removeClient(ConnectionController* p_Connection)
{
	NetworkLogger::log(NetworkLogger::Level::DEBUG, "Removing client connection from server");

	std::unique_lock<std::mutex> lock(m_ClientLock);
	for (unsigned int i = 0; i < m_ConnectedClients.size(); i++)
	{
		if (m_ConnectedClients[i].get() == p_Connection)
		{
			m_ConnectedClients.erase(m_ConnectedClients.begin() + i);
			break;
		}
	}
}
