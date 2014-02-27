#include "Connection.h"

#include "NetworkExceptions.h"
#include "NetworkLogger.h"

Connection::Connection( boost::asio::ip::tcp::socket&& p_Socket) 
		:   m_Socket(std::move(p_Socket)),
			m_LockWriting(),
			m_ReadBuffer(sizeof(Header)),
			m_SaveData(),
			m_State(State::CONNECTED)
{
}

bool Connection::isConnected() const
{
	return m_State == State::CONNECTED;
}

void Connection::disconnect()
{
	if (m_State == State::CONNECTED && m_Socket.is_open())
	{
		m_SaveData = saveDataFunction();
		m_State = State::UNCONNECTED;

		m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		m_Socket.close();
	}
}

bool Connection::hasError() const
{
	return m_State == State::INVALID;
}

void Connection::doWrite(const Header& p_Header, const std::string& p_Buffer)
{
	NetworkLogger::log(NetworkLogger::Level::TRACE, "Starting a write on a connection");

	m_WriteHeader = p_Header;
	m_WriteBuffer = p_Buffer;

	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back(boost::asio::buffer(&m_WriteHeader, sizeof(m_WriteHeader)));
	buffers.push_back(boost::asio::buffer(m_WriteBuffer));

	boost::asio::async_write(m_Socket, buffers,
		std::bind(&Connection::handleWrite, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Connection::handleWrite(const boost::system::error_code& p_Error, std::size_t /*p_BytesTransferred*/)
{
	NetworkLogger::log(NetworkLogger::Level::TRACE, "Connection handling a write response");

	if (p_Error)
	{
		if (p_Error == boost::asio::error::connection_reset
			|| p_Error == boost::asio::error::eof)
		{
			throw ClientDisconnected(formatError(p_Error), __LINE__, __FILE__);
		}
		else
		{
			throw NetworkError(formatError(p_Error), __LINE__, __FILE__);
		}
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

void Connection::readHeader()
{
	NetworkLogger::log(NetworkLogger::Level::TRACE, "Connection starting to read header");

	boost::asio::async_read(
		m_Socket,
		boost::asio::buffer(m_ReadBuffer, sizeof(Header)),
		std::bind(&Connection::handleReadHeader, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Connection::handleReadHeader(const boost::system::error_code& p_Error, std::size_t /*p_BytesTransferred*/)
{
	NetworkLogger::log(NetworkLogger::Level::TRACE, "Connection handling a read header response");

	if( p_Error )
	{
		if (m_Disconnected)
		{
			m_Disconnected();
		}

		m_State = State::INVALID;
		if (p_Error == boost::asio::error::connection_reset
			|| p_Error == boost::asio::error::eof)
		{
			throw ClientDisconnected(formatError(p_Error), __LINE__, __FILE__);
		}
		else if (p_Error == boost::asio::error::operation_aborted)
		{
			return;
		}
		else
		{
			throw NetworkError(formatError(p_Error), __LINE__, __FILE__);
		}
	}

	Header header;
	header = *((Header*)m_ReadBuffer.data());
	size_t dataSize = header.m_Size - sizeof(Header);

	if (m_ReadBuffer.size() < header.m_Size)
	{
		m_ReadBuffer.resize(header.m_Size);
	}

	boost::asio::async_read(m_Socket,
		boost::asio::buffer(m_ReadBuffer.data() + sizeof(Header), dataSize),
		std::bind(&Connection::handleReadData, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Connection::handleReadData(const boost::system::error_code& p_Error, std::size_t /*p_BytesTransferred*/) 
{
	NetworkLogger::log(NetworkLogger::Level::TRACE, "Connection handling a read data response");

	if( p_Error )
	{
		if (m_Disconnected)
		{
			m_Disconnected();
		}

		m_State = State::INVALID;
		if (p_Error == boost::asio::error::connection_reset
			|| p_Error == boost::asio::error::eof)
		{
			throw ClientDisconnected(formatError(p_Error), __LINE__, __FILE__);
		}
		else if (p_Error == boost::asio::error::operation_aborted)
		{
			return;
		}
		else
		{
			throw NetworkError(formatError(p_Error), __LINE__, __FILE__);
		}
	}

	if (m_SaveData)
	{
		Header* header = (Header*)m_ReadBuffer.data();
		std::string data(m_ReadBuffer.data() + sizeof(Header), header->m_Size - sizeof(Header));
		m_SaveData(header->m_TypeID, data);
	}

	readHeader();
}

void Connection::writeData(const std::string& p_Buffer, uint16_t p_ID)
{
	NetworkLogger::log(NetworkLogger::Level::TRACE, "Connection received data to send");

	Header header;
	header.m_Size = static_cast<uint16_t>(p_Buffer.size() + sizeof(Header));
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

void Connection::setSaveData(saveDataFunction p_SaveData)
{
	m_SaveData = p_SaveData;
}

void Connection::setDisconnectedCallback(disconnectedCallback_t p_DisconnectedCallback)
{
	m_Disconnected = p_DisconnectedCallback;
}

boost::asio::ip::tcp::socket& Connection::getSocket()
{
	return m_Socket;
}

void Connection::startReading()
{
	readHeader();
}

std::string Connection::formatError(const boost::system::error_code& p_Error)
{
	return "error: " + std::to_string(p_Error.value()) + ": " + p_Error.message();
}
