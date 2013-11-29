#include "Connection.h"

#include "MyExceptions.h"

Connection::Connection( boost::asio::ip::tcp::socket&& p_Socket) 
		:   m_Socket(std::move(p_Socket)),
			m_LockWriting(),
			m_ReadBuffer(sizeof(Header)),
			m_SaveData()
{
}

Connection::~Connection()
{
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
		std::bind(&Connection::handleWrite, this, std::placeholders::_1, std::placeholders::_2));
}

void Connection::handleWrite(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if (p_Error)
	{
		m_State = State::INVALID;
		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
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
	boost::asio::async_read(
		m_Socket,
		boost::asio::buffer(m_ReadBuffer, sizeof(Header)),
		std::bind(&Connection::handleReadHeader, this, std::placeholders::_1, std::placeholders::_2));
}

void Connection::handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred)
{
	if( p_Error )
	{
		m_State = State::INVALID;
		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
	}

	Header header;
	header = *((Header*)m_ReadBuffer.data());
	m_ReadBuffer.resize(header.m_Size);
	boost::asio::async_read(m_Socket,
		boost::asio::buffer(&m_ReadBuffer[sizeof(Header)], header.m_Size - sizeof(Header)),
		std::bind(&Connection::handleReadData, this, std::placeholders::_1, std::placeholders::_2));
}

void Connection::handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred) 
{
	if( p_Error )
	{
		m_State = State::INVALID;
		throw NetworkError(p_Error.message(), __LINE__, __FILE__);
	}

	if (m_SaveData)
	{
		Header* header = (Header*)m_ReadBuffer.data();
		std::string data(&m_ReadBuffer[sizeof(Header)], header->m_Size - sizeof(Header));
		m_SaveData(header->m_TypeID, data);
	}

	readHeader();
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

boost::asio::ip::tcp::socket* Connection::getSocket()
{
	return &m_Socket;
}

void Connection::startReading()
{
	readHeader();
}
