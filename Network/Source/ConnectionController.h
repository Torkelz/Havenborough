/**
 * File comment.
 */

#pragma once

#include "Connection.h"
#include "Packages.h"

#include <IConnectionController.h>

/**
 * Implementation of the IConnectionController interface.
 */
class ConnectionController : public IConnectionController
{
public:
	/**
	 * Unique pointer for ConnectionController objects.
	 */
	typedef std::unique_ptr<ConnectionController> ptr;

private:
	Connection::ptr m_Connection;

	const std::vector<PackageBase::ptr>& m_PackagePrototypes;
	std::vector<PackageBase::ptr> m_ReceivedPackages;
	std::mutex m_ReceivedLock;

public:
	/**
	 * constructor.
	 *
	 * Creates a ConnectionController wrapping a connection.
	 *
	 * @param p_Connection the connection to wrap.
	 * @param p_Prototypes a list of prototypes representing the supported package types.
	 */
	ConnectionController(Connection::ptr p_Connection, const std::vector<PackageBase::ptr>& p_Prototypes);

	bool isConnected() const override;
	bool hasError() const override;

	unsigned int getNumPackages() override;
	Package getPackage(unsigned int p_Index) override;
	void clearPackages(unsigned int p_NumPackages) override;

	PackageType getPackageType(Package p_Package) override;

	void sendCreateObjects(const char** p_Descriptions, unsigned int p_NumDescriptions, const ObjectInstance* p_Instances, unsigned int p_NumInstances) override;
	unsigned int getNumCreateObjectDescriptions(Package p_Package) override;
	const char* getCreateObjectDescription(Package p_Package, unsigned int p_Description) override;
	unsigned int getNumCreateObjectInstances(Package p_Package) override;
	const ObjectInstance* getCreateObjectInstances(Package p_Package) override;

	/**
	 * Start the listening loop on the connection.
	 */
	void startListening();
	/**
	 * Set a callback to get notified when the wrapped connection is disconnected.
	 *
	 * @param p_DisconnectCallback a callback function.
	 */
	void setDisconnectedCallback(Connection::disconnectedCallback_t p_DisconnectCallback);

protected:
	void writeData(const std::string& p_Buffer, uint16_t p_ID);
	void savePackageCallBack(uint16_t p_ID, const std::string& p_Data);
};
