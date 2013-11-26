#include "NetworkServer.h"

NetworkServer::NetworkServer( boost::asio::io_service& p_Service, unsigned short p_Port) 
		:	m_Acceptor(p_Service, tcp::endpoint( tcp::v4(), p_Port)),
			m_Socket(p_Service),
			m_Index(0)
{
}

void NetworkServer::start()
{
	//m_Acceptor.async_accept(m_Socket, std::bind( &NetworkServer::handleAccept, this, std::placeholders::_1));
}

void NetworkServer::stop()
{
	m_Acceptor.get_io_service().stop();
}

void NetworkServer::handleAccept( const boost::system::error_code& error)
{
	if ( error )
    {  
      std::cout << error.message() << std::endl;
      return;
    }

    readHeader();
}

void NetworkServer::readHeader()
{
	boost::asio::async_read(
		m_Socket,
		boost::asio::buffer(m_Buffer, sizeof(Header)),
		std::bind( &NetworkServer::handleReadHeader, this,
			std::placeholders::_1,
			std::placeholders::_2));
}

void NetworkServer::handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if( p_Error )
	{
		std::cout << p_Error.message() << std::endl;
		return;
	}
	Header* header = (Header*)m_Buffer.data();

	boost::asio::async_read(m_Socket, boost::asio::buffer(&m_Buffer[sizeof(Header)], header->m_Size - sizeof(Header)),
		std::bind(&NetworkServer::handleReadData, this, std::placeholders::_1,
		std::placeholders::_2));
}

void NetworkServer::handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if( p_Error)
	{
		std::cout << p_Error.message() << std::endl;
		return;
	}

	Header* header = (Header*)m_Buffer.data();
	if (header->m_TypeID == 1)
	{
		std::string message(&m_Buffer[sizeof(Header)], header->m_Size - sizeof(header));
		std::cout << message << std::endl;
	}
	//std::cout << "Received " << p_BytesTransferred << " bytes." << std::endl;

	//readHeader();
}