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
	/**
	* Connects the client to the requested URL and port.'
	*
	* @param p_IO_Service handles the async read and write operations on the connection.
	* @param p_URL the URL to connect to.
	* @param p_Port the port number to listen to.
	* @param p_ConnectionCallback callback function that returns if an connection was established.
	*/
	ClientConnect(boost::asio::io_service& p_IO_Service, const std::string& p_URL, unsigned short p_Port, connectionCallback_t p_ConnectionCallback);

	/**
	* Release the connected socket and close the connection. Use before a new connection is made.
	* @returns the closed socket.
	*/
	boost::asio::ip::tcp::socket releaseConnectedSocket();

private:
	void handleResolve(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_ResolveResult);
	void handleConnect(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_Endpoint);
};
