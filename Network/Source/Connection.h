/**
 * File comment.
 */

#pragma once

#include "IConnection.h"

#include <atomic>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <condition_variable>
#include <mutex>

/**
 * Represents a connetion to a remote computer.
 * Handles sending and receiving of raw data, prefixed with a minimal header.
 */
class Connection : public IConnection, public std::enable_shared_from_this<Connection>
{
protected:
	struct Header
	{
		uint16_t m_Size;
		uint16_t m_TypeID;
	};
	
	State m_State;

	boost::asio::ip::tcp::socket m_Socket;

	std::atomic_flag m_LockWriting;
	std::mutex m_WriteQueueLock;

	Header m_WriteHeader;
	std::string m_WriteBuffer;
	std::vector<char> m_ReadBuffer;

	std::vector<std::pair<Header, std::string>> m_WaitingToWrite;

	saveDataFunction m_SaveData;
	disconnectedCallback_t m_Disconnected;

public:
	/**
	 * Constructor.
	 *
	 * Creates a connection to manage a specific socket.
	 *
	 * @param p_Socket a connected socket that the connection
	 *			takes ownership of and manages.
	 */
	Connection( boost::asio::ip::tcp::socket&& p_Socket );

	bool isConnected() const override;
	void disconnect() override;
	bool hasError() const override;

	void writeData(const std::string& p_Buffer, uint16_t p_ID) override;
	void setSaveData(saveDataFunction p_SaveData) override;
	void setDisconnectedCallback(disconnectedCallback_t p_DisconnectedCallback) override;
	void startReading() override;

	/**
	 * Get the socket from the connection.
	 *
	 * @return the underlying socket.
	 */
	virtual boost::asio::ip::tcp::socket& getSocket();

private:
	void doWrite(const Header& p_Header, const std::string& p_Buffer);
	void handleWrite(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void readHeader();

	static std::string formatError(const boost::system::error_code& p_Error);
};
