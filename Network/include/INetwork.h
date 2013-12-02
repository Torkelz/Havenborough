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
	 * @param p_Connection The newly connected clients connection.
	 * @param p_UserData User defined data.
	 */
	typedef void (*clientConnectedCallback_t)(IConnectionController* p_Connection, void* p_UserData);

	/**
	 * Start the server at the specified port.
	 *
	 * @param p_Port, port number must be over 1024.
	 * @param p_ConnectCallback The callback to handle connected clients. Null to disable callback.
	 * @param p_UserData User defined data to be passed to the callback.
	 * @param p_NumThreads The number of worker threads to spawn. Must be at least 1.
	 */
	virtual void createServer(unsigned short p_Port, clientConnectedCallback_t p_ConnectCallback, void* p_UserData, unsigned int p_NumThreads) = 0;
	
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

	virtual IConnectionController* getConnectionToServer() = 0;
};
