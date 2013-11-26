#include "NetworkClient.h"

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

NetworkClient::NetworkClient()
	: m_Socket(m_IO_Service),
	  m_Resolver(m_IO_Service),
	  m_State(State::UNCONNECTED),
	  m_Writing(false)
{
}

NetworkClient::~NetworkClient()
{
	m_IO_Service.stop();

	if (m_IO_Thread.joinable())
	{
		m_IO_Thread.join();
	}
}

void NetworkClient::connect(const std::string& p_URL)
{
	if (m_State != State::UNCONNECTED)
	{
		std::cout << "Can only connect while not connected" << std::endl;
		return;
	}

	m_ConnectURL = p_URL;

	try
	{
		using boost::asio::ip::tcp;

		tcp::resolver::query query(tcp::v4(), m_ConnectURL, std::to_string(m_PortNumber));

		m_State = State::RESOLVING;

		std::cout << "Resolving \"" << m_ConnectURL << "\"..." << std::endl;
		m_Resolver.async_resolve(
			query,
			std::bind(
				&NetworkClient::handleResolve, this, std::placeholders::_1, std::placeholders::_2));

		m_IO_Thread.swap(std::thread(std::bind(&NetworkClient::runIO, this)));

	}
	catch (boost::system::system_error& err)
	{
		std::cout << err.what() << std::endl;
	}
}

void NetworkClient::handleResolve(const boost::system::error_code& p_Error, tcp::resolver::iterator p_ResolveResult)
{
	if (p_Error)
	{
		std::cout << p_Error.message() << std::endl;
		m_State = State::INVALID;
		return;
	}

	m_State = State::CONNECTING;

	boost::asio::async_connect(m_Socket, p_ResolveResult,
		std::bind(&NetworkClient::handleConnect, this, std::placeholders::_1, std::placeholders::_2));
}

void NetworkClient::handleConnect(const boost::system::error_code& p_Error, tcp::resolver::iterator p_Endpoint)
{
	if (p_Error)
	{
		std::cout << p_Error.message() << std::endl;
		m_State = State::INVALID;
		return;
	}

	m_State = State::CONNECTED;
	
	readHeader();

	Header* header = (Header*)m_WriteBuffer.data();
	header->m_Size = sizeof(Header) + strlen("Hello World !!!!");
	header->m_TypeID = 1;

	strcpy(&m_WriteBuffer[sizeof(Header)], "Hello World !!!!");

	m_Writing = true;
	
	boost::asio::async_write(m_Socket, boost::asio::buffer(m_WriteBuffer, header->m_Size),
		std::bind(&NetworkClient::handleWrite, this, std::placeholders::_1, std::placeholders::_2));
}

void NetworkClient::handleWrite(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if (p_Error)
	{
		std::cout << p_Error.message() << std::endl;
		m_State = State::INVALID;
		return;
	}

	m_Writing = false;

	std::cout << "Sent message" << std::endl;
}

void NetworkClient::readHeader()
{
	boost::asio::async_read(
		m_Socket,
		boost::asio::buffer(m_ReadBuffer, sizeof(Header)),
		std::bind(&NetworkClient::handleReadHeader, this, std::placeholders::_1, std::placeholders::_2));
}

void NetworkClient::handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if( p_Error )
	{
		std::cout << p_Error.message() << std::endl;
		return;
	}
	Header* header;
	header = (Header*)m_ReadBuffer.data();
	boost::asio::async_read(m_Socket,
		boost::asio::buffer(&m_ReadBuffer[sizeof(Header)], header->m_Size - sizeof(Header)),
		std::bind(&NetworkClient::handleReadData, this, std::placeholders::_1, std::placeholders::_2));
}

void NetworkClient::handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred) 
{
	if( p_Error )
	{
		std::cout << p_Error.message() << std::endl;
		return;
	}
	Header* header = (Header*)m_ReadBuffer.data();

	std::cout << "Received " << p_BytesTransferred << " bytes" << std::endl;
	std::cout << "Header Length: " << header->m_Size << " Type: " << header->m_TypeID << std::endl;
	readHeader();
}

void NetworkClient::runIO()
{
	try
	{
		m_IO_Service.run();
		std::cout << "IO service done" << std::endl;
		m_State = State::UNCONNECTED;
	}
	catch (...)
	{
		int dummy = 42;
	}
}
