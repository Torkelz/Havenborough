#pragma once

#include "MyExceptions.h"
#include "../include/INetwork.h"
#include "NetworkHandler.h"
#include "Packages.h"

class Network : public INetwork
{
private:
	NetworkHandler* m_Handler;
	boost::thread m_IOThread;
	std::vector<PackageBase::ptr> m_PackagePrototypes; 
	std::vector<PackageBase::ptr> m_ReceivedPackages;
	std::mutex m_ReceivedLock;

public:
	Network();
	~Network();

	void createServer(unsigned short p_Port) override;
	void createClient(unsigned short p_Port) override;
	void turnOfServer() override;

	bool isConnected() const override;
	bool hasError() const override;
	void connectToServer(const char* p_URL, unsigned short p_Port, actionDoneCallback p_DoneHandler, void* p_UserData) override;
	void sendAddObject(const AddObjectData& p_Data) override;

	unsigned int getNumPackages() override;
	Package getPackage(unsigned int p_Index) override;
	PackageType getPackageType(Package p_Package) override;
	void clearPackages(unsigned int p_NumPackages) override;

	AddObjectData getAddObjectData(Package p_Package) override;
	
	boost::asio::io_service& getServerService();

private:
	void savePackageCallBack(uint16_t p_ID, const std::string& p_Data);
	void registerPackages();
};