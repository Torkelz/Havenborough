#pragma once

#include "Connection.h"
#include "Packages.h"

#include <INetwork.h>

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

	unsigned int getNumPackages() override;
	Package getPackage(unsigned int p_Index) override;
	void clearPackages(unsigned int p_NumPackages) override;

	PackageType getPackageType(Package p_Package) override;

	void sendAddObject(const AddObjectData& p_Data) override;
	AddObjectData getAddObjectData(Package p_Package) override;

private:
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
	void savePackageCallBack(uint16_t p_ID, const std::string& p_Data);
};
