#include "ServerAccept.h"

#include "ConnectionController.h"
#include "MyExceptions.h"

ServerAccept::ServerAccept(boost::asio::io_service& p_IO_Service, unsigned short p_Port, std::vector<PackageBase::ptr>& p_Prototypes) 
		:	m_Acceptor(m_IO_Service, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), p_Port)),
			m_AcceptSocket(m_IO_Service),
			m_HasError(false),
			m_Running(false),
			m_PortNumber(p_Port),
			m_PackagePrototypes(p_Prototypes),
			m_IO_Service(p_IO_Service)
{
}

ServerAccept::~ServerAccept()
{
}

void ServerAccept::startServer(INetwork::clientConnectedCallback_t p_ConnectCallback, void* p_UserData, unsigned int p_NumThreads)
{
	m_ClientConnected = p_ConnectCallback;
	m_ClientConnectedUserData = p_UserData;

	try
	{
		m_Acceptor.async_accept(m_AcceptSocket, std::bind( &ServerAccept::handleAccept, this, std::placeholders::_1));
		startThreads(p_NumThreads);
	}
	catch (boost::system::system_error& err)
	{
		std::cout << err.what() << std::endl;
	}
}

void ServerAccept::stopServer()
{
	m_Running = false;
	m_Acceptor.get_io_service().stop();

	for (auto& thread : m_WorkerThreads)
	{
		thread.join();
	}
	m_WorkerThreads.clear();
}

bool ServerAccept::hasError() const
{
	return m_HasError;
}

void ServerAccept::handleAccept( const boost::system::error_code& error)
{
	if ( error )
	{  
		std::cout << error.message() << std::endl;
		return;
	}

    Connection::ptr connection(new Connection(std::move(m_AcceptSocket)));
	ConnectionController::ptr clientConnection(new ConnectionController(std::move(connection), m_PackagePrototypes));

	if (m_ClientConnected)
	{
		m_ClientConnected(clientConnection.get(), m_ClientConnectedUserData);
	}

	clientConnection->startListening();

	m_ConnectedClients.push_back(std::move(clientConnection));

	m_Acceptor.async_accept(m_AcceptSocket, std::bind( &ServerAccept::handleAccept, this, std::placeholders::_1));
}

void ServerAccept::startThreads(unsigned int p_NumThreads)
{
	if (p_NumThreads < 1)
		p_NumThreads = 1;

	m_Running = true;

	for (unsigned int i = 0; i < p_NumThreads; i++)
	{
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
			catch (ClientDisconnected& /*err*/)
			{
				//std::cout << err.what() << std::endl;
			}
			catch (NetworkError& err)
			{
				std::cout << err.what() << std::endl;
			}
		}
	}
	catch (...)
	{
		int dummy = 42;
	}
}
