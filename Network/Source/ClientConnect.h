/**
 * File comment.
 */

#pragma once

#include <CommonTypes.h>

#include <boost/asio.hpp>
#include <memory>
#include <string>

/**
 * Represents a connection attempt to a server.
 */
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
	* Attempt an asynchronous connection to a server at the requested URL and port.
	*
	* When the connection has succeeded or failed, the callback will be called with the result.
	*
	* @param p_IO_Service an IO service that can service asynchronous tasks.
	*			The caller is responsible for running the service.
	* @param p_URL the URL to connect to.
	* @param p_Port the port number on the server to connect to.
	* @param p_ConnectionCallback callback function that will be called with the result of the operation.
	*			An empty function will disable the callback.
	*/
	ClientConnect(boost::asio::io_service& p_IO_Service, const std::string& p_URL, unsigned short p_Port, connectionCallback_t p_ConnectionCallback);

	/**
	* Retreive the held socket, releasing ownership to the owner.
	*
	* Used to get the connected socket once the callback has returned with success.
	*
	* @return the socket used for the connection. The caller becomes responsible for eventually closing the socket.
	*/
	boost::asio::ip::tcp::socket releaseConnectedSocket();

private:
	void handleResolve(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_ResolveResult);
	void handleConnect(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_Endpoint);
};
