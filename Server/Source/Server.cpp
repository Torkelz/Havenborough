#include "Server.h"


void tcp_server::start()
{
	m_Acceptor.async_accept(m_Socket, boost::bind( &tcp_server::handle_accept, this, boost::asio::placeholders::error));
}

void tcp_server::handle_accept( const boost::system::error_code& error)
{
	if ( error )
    {  
      std::cout << error.message() << std::endl;
      return;
    }

    read_header();
}

void tcp_server::read_header()
{
	boost::asio::async_read(
		m_Socket,
		boost::asio::buffer(m_Buffer, HEADER_SIZE),
		boost::bind( &tcp_server::handle_read_header, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void tcp_server::handle_read_header(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if( p_Error )
	{
		std::cout << p_Error.message() << std::endl;
		return;
	}

	boost::asio::async_read(m_Socket, boost::asio::buffer(m_Buffer, DATA_SIZE),
		boost::bind(&tcp_server::handle_read_data, this, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void tcp_server::handle_read_data(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if( p_Error)
	{
		std::cout << p_Error.message() << std::endl;
		return;
	}

	read_header();
}