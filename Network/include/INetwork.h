#pragma once

#include <CommonTypes.h>

class INetwork
{
public:

	typedef unsigned int Package;

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
	* Connect the client to the server.
	* @param p_URL, address to the server.
	* @param p_Port, port number.
	*/
	virtual void connectToServer(const char* p_URL, unsigned short p_Port) = 0;

	virtual void sendAddObject(const AddObjectData& p_Data) = 0;

	virtual unsigned int getNumPackages() = 0;

	virtual Package getPackage(unsigned int p_Index) = 0;

	virtual PackageType getPackageType(Package p_Package) =0;

	virtual void clearPackages(unsigned int p_NumPackages) =0;

	virtual AddObjectData getAddObjectData(Package p_Package) =0;
};