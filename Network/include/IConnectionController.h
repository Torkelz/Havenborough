#pragma once

#include <CommonTypes.h>

class IConnectionController
{
public:
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

	virtual void startListening() = 0;

	virtual unsigned int getNumPackages() = 0;
	virtual Package getPackage(unsigned int p_Index) = 0;
	virtual void clearPackages(unsigned int p_NumPackages) = 0;

	virtual PackageType getPackageType(Package p_Package) = 0;

	virtual void sendAddObject(const AddObjectData& p_Data) = 0;
	virtual AddObjectData getAddObjectData(Package p_Package) = 0;
};