#include "ClientConnect.h"

#include "NetworkExceptions.h"
#include "NetworkLogger.h"

ClientConnect::ClientConnect(boost::asio::io_service& p_IO_Service, const std::string& p_URL, unsigned short p_Port, connectionCallback_t p_ConnectionCallback)
	:	m_IO_Service(p_IO_Service),
		m_ConnectURL(p_URL),
		m_PortNumber(p_Port),
		m_ConnectionCallback(p_ConnectionCallback),
		m_Resolver(m_IO_Service),
		m_Socket(m_IO_Service)
{
	NetworkLogger::log(NetworkLogger::Level::DEBUG_L, "Starting a client connection attempt");

	try
	{
		using boost::asio::ip::tcp;

		tcp::resolver::query query(tcp::v4(), m_ConnectURL, std::to_string(m_PortNumber));

		m_Resolver.async_resolve(
			query,
			std::bind(&ClientConnect::handleResolve, this, std::placeholders::_1, std::placeholders::_2));
	}
	catch (boost::system::system_error& err)
	{
		throw NetworkError(err.what(), __LINE__, __FILE__);
	}
}

void ClientConnect::handleResolve(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_ResolveResult)
{
	NetworkLogger::log(NetworkLogger::Level::DEBUG_L, "Handling resolve response");

	if (p_Error)
	{
		if (m_ConnectionCallback)
		{
			m_ConnectionCallback(Result::FAILURE);
			return;
		}
		else
		{
			throw NetworkError(p_Error.message(), __LINE__, __FILE__);
		}
	}

	boost::asio::async_connect(m_Socket, p_ResolveResult,
		std::bind(&ClientConnect::handleConnect, this, std::placeholders::_1, std::placeholders::_2));
}

void ClientConnect::handleConnect(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_Endpoint)
{
	NetworkLogger::log(NetworkLogger::Level::DEBUG_L, "Handling connect response");

	if (p_Error)
	{
		if (m_ConnectionCallback)
		{
			m_ConnectionCallback(Result::FAILURE);
			return;
		}
		else
		{
			throw NetworkError(p_Error.message(), __LINE__, __FILE__);
		}
	}

	if (m_ConnectionCallback)
	{
		m_ConnectionCallback(Result::SUCCESS);
	}
}

boost::asio::ip::tcp::socket ClientConnect::releaseConnectedSocket()
{
	NetworkLogger::log(NetworkLogger::Level::TRACE, "Releasing connected socket");

	return std::move(m_Socket);
}
