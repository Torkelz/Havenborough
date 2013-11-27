#pragma once
#include <boost/asio.hpp>

class INetwork
{
public:
	/**
	* Create a pointer from which the network library can be accessed.
	*/
	__declspec(dllexport) static INetwork *createNetwork();
	
	/*
	* Start the server and define the server listener port.
	* @param p_Port, port number must be over 1024.
	*/
	virtual void createServer(unsigned short p_Port) = 0;
	
	/*
	* Turn of the server.
	*/
	virtual void turnOfServer() = 0;

	/*
	* Create a send resive for the client.
	* @pragma p_Port, portnumber must be over 1024.
	*/
	virtual void createClient(unsigned short p_Port) = 0;

	/**
	* Clear sub resources allocated by the network and delete the pointer. 
	*/
	__declspec(dllexport) static void deleteNetwork(INetwork *p_Network);

	/*
	* Get the connection status for the application
	* @return state::Connected
	*/
	virtual bool isConnected() const = 0;
	/*
	* Get the connection status 
	* @return state::Invalid
	*/
	virtual bool hasError() const = 0;
	/*
	* Get the server Service for thread use.
	* @return m_IOService.
	*/
	virtual boost::asio::io_service& getServerService() = 0;

	
	virtual void connectToServer(const std::string& p_URL, unsigned short p_Port) = 0;
};