/**
 * File comment.
 */

#pragma once

#include "IConnection.h"
#include "Packages.h"

#include <IConnectionController.h>

#include <mutex>

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
	IConnection::ptr m_Connection;

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
	ConnectionController(IConnection::ptr p_Connection, const std::vector<PackageBase::ptr>& p_Prototypes);

	/**
	 * destructor.
	 */
	virtual ~ConnectionController();

	bool isConnected() const override;
	bool hasError() const override;

	unsigned int getNumPackages() override;
	Package getPackage(unsigned int p_Index) override;
	void clearPackages(unsigned int p_NumPackages) override;

	PackageType getPackageType(Package p_Package) override;

	void sendCreateObjects(const ObjectInstance* p_Instances, unsigned int p_NumInstances) override;
	unsigned int getNumCreateObjects(Package p_Package) override;
	ObjectInstance getCreateObjectDescription(Package p_Package, unsigned int p_Description) override;

	void sendUpdateObjects(const UpdateObjectData* p_ObjectData, unsigned int p_NumObjects, const char** p_ExtraData, unsigned int p_NumExtraData) override;
	unsigned int getNumUpdateObjectData(Package p_Package) override;
	const UpdateObjectData* getUpdateObjectData(Package p_Package) override;
	unsigned int getNumUpdateObjectExtraData(Package p_Package) override;
	const char* getUpdateObjectExtraData(Package p_Package, unsigned int p_ExtraData) override;

	void sendRemoveObjects(const uint32_t* p_Objects, unsigned int p_NumObjects) override;
	unsigned int getNumRemoveObjectRefs(Package p_Package) override;
	const uint32_t* getRemoveObjectRefs(Package p_Package) override;

	void sendObjectAction(uint32_t p_ObjectId, const char* p_Action) override;
	uint32_t getObjectActionId(Package p_Package) override;
	const char* getObjectActionAction(Package p_Package) override;

	void sendAssignPlayer(uint32_t p_ObjectId) override;
	uint32_t getAssignPlayerObject(Package p_Package) override;

	void sendPlayerControl(PlayerControlData p_Data) override;
	PlayerControlData getPlayerControlData(Package p_Package) override;

	void sendDoneLoading() override;

	void sendJoinGame(const char* p_Game, const char* p_Username) override;
	const char* getJoinGameName(Package p_Package) override;
	const char* getJoinGameUsername(Package p_Package) override;

	void sendGameResult(const char** p_ExtraData, unsigned int p_NumExtraData) override;
	unsigned int getNumGameResultData(Package p_Package) override;
	const char* getGameResultData(Package p_Package, unsigned int p_ExtraData) override;

	void sendLevelData(const char* p_Stream, size_t p_Size) override;
	const size_t getLevelDataSize(Package p_Package) override;
	const char* getLevelData(Package p_Package) override;

	void sendLeaveGame() override;

	void sendSetSpawnPosition(Vector3 p_Position);
	Vector3 getSetSpawnPositionData(Package p_Package);

	void sendThrowSpell(const char* p_SpellName, Vector3 p_StartPosition, Vector3 p_Direction) override;
	const char* getThrowSpellName(Package p_Package) override;
	Vector3 getThrowSpellStartPosition(Package p_Package) override;
	Vector3 getThrowSpellDirection(Package p_Package) override;

	void sendStartCountdown() override;
	void sendDoneCountdown() override;

	/**
	 * Start the listening loop on the connection.
	 */
	void startListening();
	/**
	 * Set a callback to get notified when the wrapped connection is disconnected.
	 *
	 * @param p_DisconnectCallback a callback function.
	 */
	void setDisconnectedCallback(IConnection::disconnectedCallback_t p_DisconnectCallback);

protected:
	void writeData(const std::string& p_Buffer, uint16_t p_ID);
	void savePackageCallBack(uint16_t p_ID, const std::string& p_Data);
};
