/**
 * File comment.
 */

#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <condition_variable>
#include <mutex>

/**
 * Represents a connetion to a remote computer.
 * Handles sending and receiving of raw data, prefixed with a minimal header.
 */
class Connection
{
public:
	/**
	 * Callback type used to report that a data package has been received.
	 *
	 * First argument is the id of the package, as read from the header.
	 * Second argument is the data as a string of bytes.
	 */
	typedef std::function<void(uint16_t, const std::string&)> saveDataFunction;
	/**
	 * Callback type used to report that the connection has been disconnected.
	 */
	typedef std::function<void()> disconnectedCallback_t;
	/**
	 * Unique pointer type for connection.
	 */
	typedef std::unique_ptr<Connection> ptr;

	/**
	 * Connection status.
	 */
	enum class State
	{
		UNCONNECTED,
		CONNECTED,
		INVALID,
	};

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

	std::mutex m_WaitToFree;
	std::condition_variable m_Wait;
	bool m_Reading;

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

	/**
	 * Destructor.
	 *
	 * Closes the socket and waits for any ongoing transfers to be aborted.
	 */
	~Connection();

	/**
	 * Check if the connection is currently connected.
	 *
	 * @return true if connected, otherwise false
	 */
	bool isConnected() const;
	
	/**
	 * Check if an error has been encountered.
	 *
	 * @return true if an error has occurred, otherwise false.
	 */
	bool hasError() const;

	/**
	 * Writes a buffer of data to the network stream. If the stream
	 * is busy, the data is buffered and sent when the stream has time.
	 * Data is always sent in order, even when buffered.
	 *
	 * @param p_Buffer A buffer of data to send. The data is copied and stored
	 *		internally. Therefore it is safe to delete the buffer afterwards.
	 * @param p_ID The package ID to be associated with the data.
	 */
	void writeData(const std::string& p_Buffer, uint16_t p_ID);

	/**
	 * Set a callback to handle data when received. Data is always a single complete package.
	 *
	 * @param p_SaveData the callback function to receive incoming data,
	 *			use the empty function to disable callback.
	 */
	void setSaveData(saveDataFunction p_SaveData);

	/**
	 * Set a callback to handle after the socket is disconnected.
	 */
	void setDisconnectedCallback(disconnectedCallback_t p_DisconnectedCallback);

	/**
	 * Get the socket from the connection.
	 *
	 * @return the underlying socket.
	 */
	boost::asio::ip::tcp::socket& getSocket();

	/**
	* Start a reading loop on the connection. The function should
	* not be called twice on the same connection.
	*/
	void startReading();

private:
	void doWrite(const Header& p_Header, const std::string& p_Buffer);
	static void handleWrite(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred, Connection* p_Con);
	static void handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred, Connection* p_Con);
	static void handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred, Connection* p_Con);
	void readHeader();

	static std::string formatError(const boost::system::error_code& p_Error);
};
