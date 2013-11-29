#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <mutex>

class NetworkHandler
{
private:
	typedef std::function<void(uint16_t,const std::string&)> saveDataFunction;
	enum class State
	{
		UNCONNECTED,
		RESOLVING,
		CONNECTING,
		CONNECTED,
		INVALID,
	};
	struct Header
	{
		uint16_t m_Size;
		uint16_t m_TypeID;
	};
	unsigned short m_PortNumber;

	State m_State;
	bool m_Writing;

	boost::asio::io_service m_IOService;
	boost::asio::ip::tcp::resolver m_Resolver;
	boost::asio::ip::tcp::acceptor m_Acceptor;
	boost::asio::ip::tcp::socket m_Socket;
	unsigned int m_Index;
	boost::thread m_IOThread;

	std::atomic_flag m_LockWriting;
	std::mutex m_WriteQueueLock;

	Header m_WriteHeader;
	std::string m_WriteBuffer;
	std::vector<char> m_ReadBuffer;

	std::vector<std::pair<Header, std::string>> m_WaitingToWrite;

	std::string m_ConnectURL;
	saveDataFunction m_SaveData;
public:
	/**
	* Used by the server to handle connections.
	* @param p_Port, the port number for the applicatioin to listen to.
	*/
	NetworkHandler( unsigned short p_Port );
	/**
	* Default constructor for the connecting application.
	*/
	NetworkHandler();
	/**
	* De-constructor
	*/
	~NetworkHandler();
	/*
	* Used by the application to connect to the server
	* @param p_URL, Server address, ip/url
	* "param p_Port Port to call to.
	*/
	void connectToServer(const std::string& p_URL, unsigned short p_Port);
	/*
	* Start the server up.
	*/
	void startServer();
	/*
	* Shut down the server.
	*/
	void stopServer();
	/*
	* Get the connection status for the application
	* @return state::Connected
	*/
	bool isConnected() const;
	/*
	* Get the connection status 
	* @return state::Invalid
	*/
	bool hasError() const;
	/*
	* Get the server Service for thread use.
	* @return m_IOService.
	*/
	boost::asio::io_service& getServerService();

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

private:
	void handleAccept(const boost::system::error_code& p_Error);
	void handleResolve(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_ResolveResult);
	void handleConnect(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_Endpoint);
	void doWrite(const Header& p_Header, const std::string& p_Buffer);
	void handleWrite(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void handleReadHeader(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void handleReadData(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void readHeader();
	void runIO();
};