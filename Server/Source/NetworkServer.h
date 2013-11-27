#pragma once

#include <iostream>

#include <boost/asio.hpp>

class NetworkServer : public std::enable_shared_from_this<NetworkServer>
{
private:

	struct Header
	{
		uint16_t m_Size;
		uint16_t m_TypeID;
	};
	typedef boost::asio::ip::tcp tcp;

public:
	

public:
	NetworkServer( boost::asio::io_service& p_Service, unsigned short p_Port);

	void start();
	void stop();

private:
	void handleAccept( const boost::system::error_code& p_Error);
	void readHeader();
	void handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);

private:
  tcp::acceptor m_Acceptor;
  tcp::socket m_Socket;
  unsigned int m_Index;
  std::array<char, 1024> m_Buffer;
};