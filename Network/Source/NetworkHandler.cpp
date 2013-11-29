#include "NetworkHandler.h"

#include "MyExceptions.h"

NetworkHandler::NetworkHandler(unsigned short p_Port) 
		:	m_Acceptor(m_IOService, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), p_Port)),
			m_Resolver(m_IOService),
			m_AcceptSocket(m_IOService),
			m_HasError(false)
{
}

NetworkHandler::NetworkHandler()
	:	m_Resolver(m_IOService),
		m_Acceptor(m_IOService),
		m_AcceptSocket(m_IOService),
		m_HasError(false)
{
}

NetworkHandler::~NetworkHandler()
{
	m_IOService.stop();

	if (m_IOThread.joinable())
	{
		m_IOThread.join();
	}
}

void NetworkHandler::connectToServer(const std::string& p_URL, unsigned short p_Port, actionDoneCallback p_DoneHandler, void* p_UserData)
{
	if (m_Connection)
	{
		throw NetworkError("Cannot connect again", __LINE__, __FILE__);
		return;
	}

	m_ConnectURL = p_URL;
	m_PortNumber = p_Port;
	m_DoneCallback = p_DoneHandler;
	m_CallbackUserData = p_UserData;

	try
	{
		using boost::asio::ip::tcp;

		tcp::resolver::query query(tcp::v4(), m_ConnectURL, std::to_string(m_PortNumber));

		m_Resolver.async_resolve(
			query,
			std::bind(&NetworkHandler::handleResolve, this, std::placeholders::_1, std::placeholders::_2));

		m_IOThread.swap(boost::thread(std::bind(&NetworkHandler::runIO, this)));
	}
	catch (boost::system::system_error& err)
	{
		std::cout << err.what() << std::endl;
	}
}

void NetworkHandler::startServer()
{
	try
	{
		m_Acceptor.async_accept(m_AcceptSocket, std::bind( &NetworkHandler::handleAccept, this, std::placeholders::_1));
		m_IOThread.swap(boost::thread(std::bind(&NetworkHandler::runIO, this)));
	}
	catch (boost::system::system_error& err)
	{
		std::cout << err.what() << std::endl;
	}
}

void NetworkHandler::stopServer()
{
	m_Acceptor.get_io_service().stop();
}

bool NetworkHandler::isConnected() const
{
	return m_Connection;
}

bool NetworkHandler::hasError() const
{
	return m_HasError;
}

void NetworkHandler::handleResolve(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_ResolveResult)
{
	if (p_Error)
	{
		if (m_DoneCallback)
		{
			m_DoneCallback(Result::FAILURE, m_CallbackUserData);
		}

		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
	}

	m_AcceptSocket = boost::asio::ip::tcp::socket(m_IOService);
	boost::asio::async_connect(m_AcceptSocket, p_ResolveResult,
		std::bind(&NetworkHandler::handleConnect, this, std::placeholders::_1, std::placeholders::_2));
}

void NetworkHandler::handleConnect(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_Endpoint)
{
	if (p_Error)
	{
		if (m_DoneCallback)
		{
			m_DoneCallback(Result::FAILURE, m_CallbackUserData);
		}

		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
	}
	
	m_Connection.reset();
	m_Connection.reset(new Connection(std::move(m_AcceptSocket)));
	m_Connection->setSaveData(m_SaveFunction);
	
	m_Connection->startReading();

	if (m_DoneCallback)
	{
		m_DoneCallback(Result::SUCCESS, m_CallbackUserData);
	}
}

void NetworkHandler::handleAccept( const boost::system::error_code& error)
{
	if ( error )
	{  
		std::cout << error.message() << std::endl;
		return;
	}

    m_Connection.reset();
	m_Connection.reset(new Connection(std::move(m_AcceptSocket)));
	m_Connection->setSaveData(m_SaveFunction);

	m_Connection->startReading();
}

void NetworkHandler::runIO()
{
	try
	{
		boost::system::error_code error;
		m_IOService.run(error);
	}
	catch (...)
	{
		int dummy = 42;
	}
}

boost::asio::io_service& NetworkHandler::getServerService()
{
	return m_IOService;
}

void NetworkHandler::writeData(const std::string& p_Buffer, uint16_t p_ID)
{
	if (m_Connection)
	{
		m_Connection->writeData(p_Buffer, p_ID);
	}
}

void NetworkHandler::setSaveData(Connection::saveDataFunction p_SaveData)
{
	m_SaveFunction = p_SaveData;
}