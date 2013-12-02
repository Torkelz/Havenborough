#pragma once

#include <CommonTypes.h>

#include <boost/asio.hpp>
#include <memory>
#include <string>

class ClientConnect
{
private:
	std::string m_ConnectURL;
	unsigned short m_PortNumber;

	boost::asio::io_service& m_IO_Service;
	boost::asio::ip::tcp::resolver m_Resolver;
	boost::asio::ip::tcp::socket m_Socket;

	typedef std::function<void(Result)> connectionCallback_t;
	connectionCallback_t m_ConnectionCallback;

public:
	ClientConnect(boost::asio::io_service& p_IO_Service, const std::string& p_URL, unsigned short p_Port, connectionCallback_t p_ConnectionCallback);

	boost::asio::ip::tcp::socket releaseConnectedSocket();

private:
	void handleResolve(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_ResolveResult);
	void handleConnect(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_Endpoint);
};
