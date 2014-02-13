/**
 * File comment.
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>

/**
 * Interface for a connetion to a remote computer.
 */
class IConnection
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
	 * Smart pointer type for connection.
	 */
	typedef std::shared_ptr<IConnection> ptr;

	/**
	 * Connection status.
	 */
	enum class State
	{
		UNCONNECTED,
		CONNECTED,
		INVALID,
	};

public:
	virtual ~IConnection() {};

	/**
	 * Check if the connection is currently connected.
	 *
	 * @return true if connected, otherwise false
	 */
	virtual bool isConnected() const = 0;

	/**
	 * Disconnect the connection if connected.
	 */
	virtual void disconnect() = 0;
	
	/**
	 * Check if an error has been encountered.
	 *
	 * @return true if an error has occurred, otherwise false.
	 */
	virtual bool hasError() const = 0;

	/**
	 * Writes a buffer of data to the network stream. If the stream
	 * is busy, the data is buffered and sent when the stream has time.
	 * Data is always sent in order, even when buffered.
	 *
	 * @param p_Buffer A buffer of data to send. The data is copied and stored
	 *		internally. Therefore it is safe to delete the buffer afterwards.
	 * @param p_ID The package ID to be associated with the data.
	 */
	virtual void writeData(const std::string& p_Buffer, uint16_t p_ID) = 0;

	/**
	 * Set a callback to handle data when received. Data is always a single complete package.
	 *
	 * @param p_SaveData the callback function to receive incoming data,
	 *			use the empty function to disable callback.
	 */
	virtual void setSaveData(saveDataFunction p_SaveData) = 0;

	/**
	 * Set a callback to handle after the socket is disconnected.
	 */
	virtual void setDisconnectedCallback(disconnectedCallback_t p_DisconnectedCallback) = 0;

	///**
	// * Get the socket from the connection.
	// *
	// * @return the underlying socket.
	// */
	//virtual boost::asio::ip::tcp::socket& getSocket();

	/**
	* Start a reading loop on the connection. The function should
	* not be called twice on the same connection.
	*/
	virtual void startReading() = 0;
};
