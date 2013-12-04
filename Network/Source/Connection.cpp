#include "Connection.h"

#include "MyExceptions.h"

Connection::Connection( boost::asio::ip::tcp::socket&& p_Socket) 
		:   m_Socket(std::move(p_Socket)),
			m_LockWriting(),
			m_ReadBuffer(sizeof(Header)),
			m_SaveData(),
			m_Reading(false)
{
}

Connection::~Connection()
{
	m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	m_Socket.close();

	std::unique_lock<std::mutex> lock(m_WaitToFree);
	while (m_Reading)
	{
		m_Wait.wait(lock);
	}
}

bool Connection::isConnected() const
{
	return m_State == State::CONNECTED;
}

bool Connection::hasError() const
{
	return m_State == State::INVALID;
}

void Connection::doWrite(const Header& p_Header, const std::string& p_Buffer)
{
	m_WriteHeader = p_Header;
	m_WriteBuffer = p_Buffer;

	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back(boost::asio::buffer(&m_WriteHeader, sizeof(m_WriteHeader)));
	buffers.push_back(boost::asio::buffer(m_WriteBuffer));

	boost::asio::async_write(m_Socket, buffers,
		std::bind(&Connection::handleWrite, std::placeholders::_1, std::placeholders::_2, this));
}

void Connection::handleWrite(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred, Connection* p_Con)
{
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

	std::lock_guard<std::mutex> lock(p_Con->m_WriteQueueLock);
	if (!p_Con->m_WaitingToWrite.empty())
	{
		p_Con->doWrite(p_Con->m_WaitingToWrite[0].first, p_Con->m_WaitingToWrite[0].second);
		p_Con->m_WaitingToWrite.erase(p_Con->m_WaitingToWrite.begin());
	}
	else
	{
		p_Con->m_LockWriting.clear();
	}
}

void Connection::readHeader()
{
	boost::asio::async_read(
		m_Socket,
		boost::asio::buffer(m_ReadBuffer, sizeof(Header)),
		std::bind(&Connection::handleReadHeader, std::placeholders::_1, std::placeholders::_2, this));
}

void Connection::handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred, Connection* p_Con)
{
	if( p_Error )
	{
		if (p_Con->m_Disconnected)
		{
			p_Con->m_Disconnected();
		}

		p_Con->m_State = State::INVALID;
		p_Con->m_Reading = false;
		p_Con->m_Wait.notify_one();
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
	header = *((Header*)p_Con->m_ReadBuffer.data());
	p_Con->m_ReadBuffer.resize(header.m_Size);
	boost::asio::async_read(p_Con->m_Socket,
		boost::asio::buffer(&p_Con->m_ReadBuffer[sizeof(Header)], header.m_Size - sizeof(Header)),
		std::bind(&Connection::handleReadData, std::placeholders::_1, std::placeholders::_2, p_Con));
}

void Connection::handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred, Connection* p_Con) 
{
	if( p_Error )
	{
		if (p_Con->m_Disconnected)
		{
			p_Con->m_Disconnected();
		}

		p_Con->m_State = State::INVALID;
		p_Con->m_Reading = false;
		p_Con->m_Wait.notify_one();
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

	if (p_Con->m_SaveData)
	{
		Header* header = (Header*)p_Con->m_ReadBuffer.data();
		std::string data(&p_Con->m_ReadBuffer[sizeof(Header)], header->m_Size - sizeof(Header));
		p_Con->m_SaveData(header->m_TypeID, data);
	}

	p_Con->readHeader();
}

void Connection::writeData(const std::string& p_Buffer, uint16_t p_ID)
{
	Header header;
	header.m_Size = p_Buffer.size() + sizeof(Header);
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
	m_Reading = true;
	readHeader();
}

std::string Connection::formatError(const boost::system::error_code& p_Error)
{
	return "error: " + std::to_string(p_Error.value()) + ": " + p_Error.message();
}
