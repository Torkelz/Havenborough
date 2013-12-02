#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <condition_variable>
#include <mutex>

class Connection
{
public:
	typedef std::function<void(uint16_t,const std::string&)> saveDataFunction;
	typedef std::unique_ptr<Connection> ptr;

	enum class State
	{
		UNCONNECTED,
		CONNECTED,
		INVALID,
	};

private:
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

	std::mutex m_WaitToFree;
	std::condition_variable m_Wait;
	bool m_Reading;

	saveDataFunction m_SaveData;

public:

	/**
	* Used by the server to handle connections.
	* @param p_Port, the port number for the applicatioin to listen to.
	*/
	Connection( boost::asio::ip::tcp::socket&& p_Socket );

	/**
	* De-constructor
	*/
	~Connection();

	/**
	* Get the connection status for the application
	* @return state::Connected
	*/
	bool isConnected() const;
	
	/**
	* Get the connection status 
	* @return state::Invalid
	*/
	bool hasError() const;

	/**
	 * Writes a buffer of data to the network stream. If the stream
	 * is busy, the data is buffered and sent when the stream has time.
	 * Data is always sent in order, even when buffered.
	 *
	 * @param p_Buffer A buffer of data to send. The data is copied and stored
	 *		internally. Therefore it is safe to delete the buffer afterwards.
	 * @param p_ID The package ID associated with the data.
	 */
	void writeData(const std::string& p_Buffer, uint16_t p_ID);

	/**
	* Set a callback to handle data when received.
	* @param p_SaveData is the callback function, use the empty function to disable callback.
	*/
	void setSaveData(saveDataFunction p_SaveData);

	boost::asio::ip::tcp::socket* getSocket();

	void startReading();

private:
	void doWrite(const Header& p_Header, const std::string& p_Buffer);
	static void handleWrite(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred, Connection* p_Con);
	static void handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred, Connection* p_Con);
	static void handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred, Connection* p_Con);
	void readHeader();

	static std::string formatError(const boost::system::error_code& p_Error);
};
