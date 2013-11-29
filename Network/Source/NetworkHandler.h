#pragma once

#include "Connection.h"
#include <CommonTypes.h>

#include <atomic>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <mutex>

class NetworkHandler
{
private:
	unsigned short m_PortNumber;
	std::string m_ConnectURL;

	boost::asio::io_service m_IOService;
	boost::asio::ip::tcp::resolver m_Resolver;
	boost::asio::ip::tcp::acceptor m_Acceptor;
	boost::asio::ip::tcp::socket m_AcceptSocket;
	boost::thread m_IOThread;

	Connection::ptr m_Connection;
	bool m_HasError;

	actionDoneCallback m_DoneCallback;
	void* m_CallbackUserData;

	Connection::saveDataFunction m_SaveFunction;

public:
	/**
	* Used by the server to handle connections.
	* @param p_Port, the port number for the applicatioin to listen to.
	*/
	NetworkHandler( unsigned short p_Port );
	/**
	* De-constructor
	*/
	~NetworkHandler();
	/*
	* Used by the application to connect to the server
	* @param p_URL, Server address, ip/url
	* "param p_Port Port to call to.
	*/
	void connectToServer(const std::string& p_URL, actionDoneCallback p_DoneHandler, void* p_UserData);
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
	* Set a callback to handle data when received.
	* @param p_SaveData is the callback function, use the empty function to disable callback.
	*/
	void setSaveData(Connection::saveDataFunction p_SaveData);

	Connection::ptr getClientConnection();

private:
	void handleAccept(const boost::system::error_code& p_Error);
	void handleResolve(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_ResolveResult);
	void handleConnect(const boost::system::error_code& p_Error, boost::asio::ip::tcp::resolver::iterator p_Endpoint);
	void runIO();
};