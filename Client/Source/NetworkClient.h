#pragma once

#include <boost/asio.hpp>
#include <thread>

class NetworkClient
{
private:
	enum class State
	{
		UNCONNECTED,
		RESOLVING,
		CONNECTING,
		CONNECTED,
		INVALID,
	};
	struct Header
	{
		uint16_t m_Size;
		uint16_t m_TypeID;
	};

	static const unsigned short m_PortNumber = 31415;

	State m_State;
	bool m_Writing;

	boost::asio::io_service			m_IO_Service;
	boost::asio::ip::tcp::resolver	m_Resolver;
	boost::asio::ip::tcp::socket	m_Socket;

	std::thread m_IO_Thread;

	std::array<char, 1024> m_WriteBuffer;
	std::array<char, 1024> m_ReadBuffer;

	std::string m_ConnectURL;

public:
	NetworkClient();
	~NetworkClient();

	void connect(const std::string& p_URL);

private:
	void handleResolve(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_ResolveResult);
	void handleConnect(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_Endpoint);
	void handleWrite(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void readHeader();
	void runIO();
};
