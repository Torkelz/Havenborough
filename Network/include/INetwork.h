#pragma once

#include <CommonTypes.h>
#include <IConnectionController.h>

class INetwork
{
public:

	/**
	* Create a pointer from which the network library can be accessed.
	*/
	__declspec(dllexport) static INetwork *createNetwork();
	
	/**
	 * Callback for successful client connections.
	 *
	 * @param p_Connection the newly connected clients connection.
	 * @param p_UserData user defined data.
	 */
	typedef void (*clientConnectedCallback_t)(IConnectionController* p_Connection, void* p_UserData);

	/**
	* Callback for successful client connections.
	*
	* @param p_Connection the newly connected clients connection.
	* @param p_UserData user defined data.
	*/
	typedef void (*clientDisconnectedCallback_t)(IConnectionController* p_Connection, void* p_UserData);

	/**
	* Create the server at the specified port.
	*
	* @param p_Port, port number must be over 1024.
	*/
	virtual void createServer(unsigned short p_Port) = 0;
 
	/** 
	* Start the server and define the number of threads.
	* 
	* @param p_NumThreads The number of worker threads to spawn. Must be at least 1.
	*/
	virtual void startServer(unsigned int p_NumThreads) = 0;

	/**
	* 
	*
	* @param p_ConnectCallback The callback to handle connected clients. Null to disable callback.
	* @param p_UserData User defined data to be passed to the callback.
	*/
	virtual void setClientConnectedCallback(clientConnectedCallback_t p_ConnectCallback, void* p_UserData) = 0;

	/**
	* 
	*
	* @param p_DisconnectCallback The callback to handle connected clients. Null to disable callback.
	* @param p_UserData User defined data to be passed to the callback.
	*/
	virtual void setClientDisconnectedCallback(clientDisconnectedCallback_t p_DisconnectCallback, void* p_UserData) = 0;

	/*
	* Turn of the server.
	*/
	virtual void turnOfServer() = 0;

	/**
	* Clear sub resources allocated by the network and delete the pointer. 
	*/
	__declspec(dllexport) static void deleteNetwork(INetwork *p_Network);

	/*
	* Connect the client to the server.
	*
	* @param p_URL, address to the server.
	* @param p_Port Port number on the server to connect to.
	*/
	virtual void connectToServer(const char* p_URL, unsigned short p_Port, actionDoneCallback p_DoneHandler, void* p_UserData) = 0;

	/**
	* Returns the Connection from the server.
	*
	* @returns a Connection pointer.
	*/
	virtual IConnectionController* getConnectionToServer() = 0;
};
