#pragma once

#include "Connection.h"
#include "Packages.h"

#include <IConnectionController.h>

class ConnectionController : public IConnectionController
{
public:
	typedef std::unique_ptr<ConnectionController> ptr;

private:
	Connection::ptr m_Connection;

	const std::vector<PackageBase::ptr>& m_PackagePrototypes;
	std::vector<PackageBase::ptr> m_ReceivedPackages;
	std::mutex m_ReceivedLock;

public:
	ConnectionController(Connection::ptr p_Connection, const std::vector<PackageBase::ptr>& p_Prototypes);

	bool isConnected() const override;
	bool hasError() const override;
	void startListening() override;

	unsigned int getNumPackages() override;
	Package getPackage(unsigned int p_Index) override;
	void clearPackages(unsigned int p_NumPackages) override;

	PackageType getPackageType(Package p_Package) override;

	void sendAddObject(const AddObjectData& p_Data) override;
	AddObjectData getAddObjectData(Package p_Package) override;

	void setDisconnectedCallback(Connection::disconnectedCallback_t p_DisconnectCallback);

private:
	void writeData(const std::string& p_Buffer, uint16_t p_ID);
	void savePackageCallBack(uint16_t p_ID, const std::string& p_Data);
};
