#include "NetworkHandler.h"

#include "MyExceptions.h"

NetworkHandler::NetworkHandler(unsigned short p_Port) 
		:	m_Acceptor(m_IOService, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), p_Port)),
			m_Socket(m_IOService),
			m_Index(0),
			m_Resolver(m_IOService)
{
}

NetworkHandler::NetworkHandler()
	:	m_Resolver(m_IOService),
		m_Socket(m_IOService),
		m_Index(0),
		m_State(State::UNCONNECTED),
		m_Writing(false),
		m_Acceptor(m_IOService)
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

void NetworkHandler::connectToServer(const std::string& p_URL)
{
	if (m_State != State::UNCONNECTED)
	{
		throw NetworkError("Cannot connect again", __LINE__, __FILE__);
		return;
	}

	m_ConnectURL = p_URL;

	try
	{
		using boost::asio::ip::tcp;

		tcp::resolver::query query(tcp::v4(), m_ConnectURL, std::to_string(m_PortNumber));

		m_State = State::RESOLVING;

		m_Resolver.async_resolve(
			query,
			std::bind(
				&NetworkHandler::handleResolve, this, std::placeholders::_1, std::placeholders::_2));

		m_IOThread.swap(boost::thread(std::bind(&NetworkHandler::runIO, this)));
	}
	catch (boost::system::system_error& err)
	{
		std::cout << err.what() << std::endl;
	}
}

void NetworkHandler::startServer()
{
	m_Acceptor.async_accept(m_Socket, std::bind( &NetworkHandler::handleAccept, this, std::placeholders::_1));
}

void NetworkHandler::stopServer()
{
	m_Acceptor.get_io_service().stop();
}

bool NetworkHandler::isConnected() const
{
	return m_State == State::CONNECTED;
}

bool NetworkHandler::hasError() const
{
	return m_State == State::INVALID;
}

void NetworkHandler::handleResolve(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_ResolveResult)
{
	if (p_Error)
	{
		m_State = State::INVALID;
		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
	}

	m_State = State::CONNECTING;

	boost::asio::async_connect(m_Socket, p_ResolveResult,
		std::bind(&NetworkHandler::handleConnect, this, std::placeholders::_1, std::placeholders::_2));
}

void NetworkHandler::handleConnect(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_Endpoint)
{
	if (p_Error)
	{
		m_State = State::INVALID;
		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
	}

	m_State = State::CONNECTED;
	
	readHeader();
}

void NetworkHandler::handleWrite(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if (p_Error)
	{
		m_State = State::INVALID;
		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
	}

	m_Writing = false;

	std::cout << "Sent message" << std::endl;
}

void NetworkHandler::readHeader()
{
	boost::asio::async_read(
		m_Socket,
		boost::asio::buffer(m_ReadBuffer, sizeof(Header)),
		std::bind(&NetworkHandler::handleReadHeader, this, std::placeholders::_1, std::placeholders::_2));
}

void NetworkHandler::handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if( p_Error )
	{
		m_State = State::INVALID;
		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
	}

	Header* header;
	header = (Header*)m_ReadBuffer.data();
	boost::asio::async_read(m_Socket,
		boost::asio::buffer(&m_ReadBuffer[sizeof(Header)], header->m_Size - sizeof(Header)),
		std::bind(&NetworkHandler::handleReadData, this, std::placeholders::_1, std::placeholders::_2));
}

void NetworkHandler::handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred) 
{
	if( p_Error )
	{
		m_State = State::INVALID;
		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
	}
	Header* header = (Header*)m_ReadBuffer.data();

	std::cout << "Received " << p_BytesTransferred << " bytes" << std::endl;
	std::cout << "Header Length: " << header->m_Size << " Type: " << header->m_TypeID << std::endl;
	readHeader();
}

void NetworkHandler::handleAccept( const boost::system::error_code& error)
{
	if ( error )
    {  
      std::cout << error.message() << std::endl;
      return;
    }

    readHeader();
}

void NetworkHandler::runIO()
{
	try
	{
		boost::system::error_code error;
		m_IOService.run(error);
		//std::cout << "IO service done" << std::endl;
		m_State = State::UNCONNECTED;
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