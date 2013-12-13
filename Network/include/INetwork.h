/**
 * File comment.
 */
#pragma once

#include <CommonTypes.h>
#include <IConnectionController.h>

/**
 * Interface for connecting a server and multiple clients.
 */
class INetwork
{
public:
	/**
	 * Callback for successful client connections.
	 *
	 * @param p_Connection the newly connected clien'ts connection.
	 * @param p_UserData user defined data.
	 */
	typedef void (*clientConnectedCallback_t)(IConnectionController* p_Connection, void* p_UserData);

	/**
	 * Callback for successful client connections.
	 *
	 * @param p_Connection the disconnected client's connection.
	 * @param p_UserData user defined data.
	 */
	typedef void (*clientDisconnectedCallback_t)(IConnectionController* p_Connection, void* p_UserData);

	/**
	 * Callback for logging.
	 *
	 * @param p_Level log priority level. Higher is more important.
	 * @param p_Message the log message.
	 */
	typedef void (*clientLogCallback_t)(uint32_t p_Level, const char* p_Message);

	/**
	 * Create an instance of the network library.
	 *
	 * Use {@link #deleteNetwork(INetwork*)} to clean up.
	 *
	 * @return a pointer to the network library implementation.
	 */
	__declspec(dllexport) static INetwork *createNetwork();

	/**
	 * Initialize the library. Must be called before any other function.
	 */
	virtual void initialize() = 0;
	
	/**
	 * Create the server at the specified port.
	 * Only one server per network may be created.
	 *
	 * @param p_Port the port number to listen for client connections on.
	 *			Depending on system, ports below 1025 may require special user privileges.
	 */
	virtual void createServer(unsigned short p_Port) = 0;
 
	/** 
	 * Start the server with a specified number of worker threads.
	 *
	 * The server must have been previously created using {@link #createServer(unsigned short)}.
	 * 
	 * @param p_NumThreads The number of worker threads to spawn. Must be at least 1.
	 */
	virtual void startServer(unsigned int p_NumThreads) = 0;

	/**
	 * Set a callback to get notified when a client has connected.
	 *
	 * The callback is called after the connection has been established, but before listening for data starts.
	 *
	 * @param p_ConnectCallback a callback to handle connected clients. Null to disable callback.
	 * @param p_UserData User defined data to be passed unmodified to the callback.
	 */
	virtual void setClientConnectedCallback(clientConnectedCallback_t p_ConnectCallback, void* p_UserData) = 0;

	/**
	 * Set a callback to get notified when a client has disconnected.
	 *
	 * The callback is called after the client has disconnected, but before the connection is removed.
	 *
	 * @param p_DisconnectCallback The callback to handle connected clients. Null to disable callback.
	 * @param p_UserData User defined data to be passed to the callback.
	 */
	virtual void setClientDisconnectedCallback(clientDisconnectedCallback_t p_DisconnectCallback, void* p_UserData) = 0;

	/**
	 * Turn off the server.
	 *
	 * Clients are disconnected and worker threads are stopped.
	 */
	virtual void turnOffServer() = 0;

	/**
	 * Clear sub resources allocated by the network and delete the library. 
	 */
	__declspec(dllexport) static void deleteNetwork(INetwork *p_Network);

	/**
	 * Connect as a client to a server.
	 *
	 * @param p_URL address to a server.
	 * @param p_Port port number on the server to connect to.
	 * @param p_DoneHandler callback for reporting when a connection has succeeded or failed.
	 * @param p_UserData user defined data to be passed unmodified to the callback function.
	 */
	virtual void connectToServer(const char* p_URL, unsigned short p_Port, actionDoneCallback p_DoneHandler, void* p_UserData) = 0;

	/**
	 * Get a controller to the client connection to the server.
	 *
	 * Should not be used before the callback in
	 * {@link #connectToServer(const char*, unsigned short, actionDoneCallback, void*)}
	 * has reported that a connection has successfully been established.
	 *
	 * @return a controller for sending and receiving data from the server.
	 */
	virtual IConnectionController* getConnectionToServer() = 0;

	/**
	 * Set the function to handle log messages.
	 *
	 * @param p_LogCallback the function to be called whenever a message is to
	 *			be logged from this component. Set to null to disable logging.
	 */
	virtual void setLogFunction(clientLogCallback_t p_LogCallback) = 0;

protected:
	virtual ~INetwork() {};
};
