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

void NetworkHandler::connectToServer(const std::string& p_URL, unsigned short p_Port)
{
	if (m_State != State::UNCONNECTED)
	{
		throw NetworkError("Cannot connect again", __LINE__, __FILE__);
		return;
	}

	m_ConnectURL = p_URL;
	m_PortNumber = p_Port;

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
	try
	{
		m_Acceptor.async_accept(m_Socket, std::bind( &NetworkHandler::handleAccept, this, std::placeholders::_1));
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

void NetworkHandler::writeData(const std::string& p_Buffer, uint16_t p_ID)
{
	Header header;
	header.m_Size = m_WriteBuffer.size() + sizeof(Header);
	header.m_TypeID = p_ID;

	if(!m_LockWriting.test_and_set())
	{
		doWrite(header, p_Buffer);
	}
	else
	{
		std::lock_guard<std::mutex> lock(m_WriteQueueLock);
		m_WaitingToWrite.push_back(std::make_pair(header, p_Buffer));
	}
}

void NetworkHandler::doWrite(const Header& p_Header, const std::string& p_Buffer)
{
	m_headerWrite = p_Header;
	m_WriteBuffer = p_Buffer;

	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back(boost::asio::buffer(&m_headerWrite, sizeof(m_headerWrite)));
	buffers.push_back(boost::asio::buffer(m_WriteBuffer));

	boost::asio::async_write(m_Socket, buffers, 
		std::bind(&NetworkHandler::handleWrite, this, std::placeholders::_1, std::placeholders::_2));
}

void NetworkHandler::handleWrite(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if (p_Error)
	{
		m_State = State::INVALID;
		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
	}

	std::lock_guard<std::mutex> lock(m_WriteQueueLock);
	if (!m_WaitingToWrite.empty())
	{
		doWrite(m_WaitingToWrite[0].first, m_WaitingToWrite[0].second);
		m_WaitingToWrite.erase(m_WaitingToWrite.begin());
	}
	else
	{
		m_LockWriting.clear();
	}
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