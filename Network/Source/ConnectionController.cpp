#include "ConnectionController.h"

#include "NetworkLogger.h"

ConnectionController::ConnectionController(IConnection::ptr p_Connection, const std::vector<PackageBase::ptr>& p_Prototypes)
	:	m_PackagePrototypes(p_Prototypes),
		m_Connection(std::move(p_Connection))
{
	NetworkLogger::log(NetworkLogger::Level::DEBUG_L, "Creating a connection controller");

	m_Connection->setSaveData(std::bind(&ConnectionController::savePackageCallBack, this, std::placeholders::_1, std::placeholders::_2));
}

ConnectionController::~ConnectionController()
{
	m_Connection->disconnect();
}

bool ConnectionController::isConnected() const
{
	return m_Connection->isConnected();
}

bool ConnectionController::hasError() const
{
	return m_Connection->hasError();
}

void ConnectionController::startListening()
{
	m_Connection->startReading();
}

unsigned int ConnectionController::getNumPackages()
{
	return m_ReceivedPackages.size();
}

Package ConnectionController::getPackage(unsigned int p_Index)
{
	return p_Index;
}

void ConnectionController::clearPackages(unsigned int p_NumPackages)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	m_ReceivedPackages.erase(m_ReceivedPackages.begin(), m_ReceivedPackages.begin() + p_NumPackages);
}

PackageType ConnectionController::getPackageType(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	if (m_ReceivedPackages.size() > p_Package)
		return m_ReceivedPackages[p_Package]->getType();
	else
		return PackageType::RESERVED;
}

void ConnectionController::sendCreateObjects(const ObjectInstance* p_Instances, unsigned int p_NumInstances)
{
	CreateObjects package;
	for (unsigned int i = 0; i < p_NumInstances; ++i)
	{
		package.m_Object1.push_back(std::make_pair(std::string(p_Instances[i].m_Description), p_Instances[i].m_Id));
	}

	writeData(package.getData(), (uint16_t)package.getType());
}

unsigned int ConnectionController::getNumCreateObjects(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	CreateObjects* createObjects = static_cast<CreateObjects*>(m_ReceivedPackages[p_Package].get());
	return createObjects->m_Object1.size();
}

ObjectInstance ConnectionController::getCreateObjectDescription(Package p_Package, unsigned int p_Description)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	CreateObjects* createObjects = static_cast<CreateObjects*>(m_ReceivedPackages[p_Package].get());
	ObjectInstance inst;
	inst.m_Description = createObjects->m_Object1[p_Description].first.c_str();
	inst.m_Id = createObjects->m_Object1[p_Description].second;
	return inst;
}

void ConnectionController::sendUpdateObjects(const UpdateObjectData* p_ObjectData, unsigned int p_NumObjects, const char** p_ExtraData, unsigned int p_NumExtraData)
{
	UpdateObjects package;
	for (unsigned int i = 0; i < p_NumExtraData; ++i)
	{
		package.m_Object2.push_back(std::string(p_ExtraData[i]));
	}
	package.m_Object1.assign(p_ObjectData, p_ObjectData + p_NumObjects);

	writeData(package.getData(), (uint16_t)package.getType());
}

unsigned int ConnectionController::getNumUpdateObjectData(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	UpdateObjects* createObjects = static_cast<UpdateObjects*>(m_ReceivedPackages[p_Package].get());
	return createObjects->m_Object1.size();
}

const UpdateObjectData* ConnectionController::getUpdateObjectData(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	UpdateObjects* createObjects = static_cast<UpdateObjects*>(m_ReceivedPackages[p_Package].get());
	return createObjects->m_Object1.data();
}

unsigned int ConnectionController::getNumUpdateObjectExtraData(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	UpdateObjects* createObjects = static_cast<UpdateObjects*>(m_ReceivedPackages[p_Package].get());
	return createObjects->m_Object2.size();
}

const char* ConnectionController::getUpdateObjectExtraData(Package p_Package, unsigned int p_ExtraData)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	UpdateObjects* createObjects = static_cast<UpdateObjects*>(m_ReceivedPackages[p_Package].get());
	return createObjects->m_Object2[p_ExtraData].c_str();
}

void ConnectionController::sendRemoveObjects(const uint32_t* p_Objects, unsigned int p_NumObjects)
{
	RemoveObjects package;
	package.m_Object1.assign(p_Objects, p_Objects + p_NumObjects);

	writeData(package.getData(), (uint16_t)package.getType());
}

unsigned int ConnectionController::getNumRemoveObjectRefs(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	RemoveObjects* removeObjects = static_cast<RemoveObjects*>(m_ReceivedPackages[p_Package].get());
	return removeObjects->m_Object1.size();
}

const uint32_t* ConnectionController::getRemoveObjectRefs(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	RemoveObjects* removeObjects = static_cast<RemoveObjects*>(m_ReceivedPackages[p_Package].get());
	return removeObjects->m_Object1.data();
}

void ConnectionController::sendObjectAction(uint32_t p_ObjectId, const char* p_Action)
{
	ObjectAction package;
	package.m_Object1 = p_ObjectId;
	package.m_Object2 = p_Action;

	writeData(package.getData(), (uint16_t)package.getType());
}

uint32_t ConnectionController::getObjectActionId(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	ObjectAction* objectAction = static_cast<ObjectAction*>(m_ReceivedPackages[p_Package].get());
	return objectAction->m_Object1;
}

const char* ConnectionController::getObjectActionAction(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	ObjectAction* objectAction = static_cast<ObjectAction*>(m_ReceivedPackages[p_Package].get());
	return objectAction->m_Object2.c_str();
}

void ConnectionController::sendAssignPlayer(uint32_t p_ObjectId)
{
	AssignPlayer package;
	package.m_Object1 = p_ObjectId;

	writeData(package.getData(), (uint16_t)package.getType());
}

uint32_t ConnectionController::getAssignPlayerObject(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	AssignPlayer* assignPlayer = static_cast<AssignPlayer*>(m_ReceivedPackages[p_Package].get());
	return assignPlayer->m_Object1;
}

void ConnectionController::sendPlayerControl(PlayerControlData p_Data)
{
	PlayerControl package;
	package.m_Object1 = p_Data;

	writeData(package.getData(), (uint16_t)package.getType());
}

PlayerControlData ConnectionController::getPlayerControlData(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	PlayerControl* playerControl = static_cast<PlayerControl*>(m_ReceivedPackages[p_Package].get());
	return playerControl->m_Object1;
}

void ConnectionController::sendDoneLoading()
{
	DoneLoading package;
	writeData(package.getData(), (uint16_t)package.getType());
}

void ConnectionController::sendJoinGame(const char* p_Game)
{
	JoinGame package;
	package.m_Object1 = p_Game;

	writeData(package.getData(), (uint16_t)package.getType());
}

const char* ConnectionController::getJoinGameName(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	JoinGame* joinGame = static_cast<JoinGame*>(m_ReceivedPackages[p_Package].get());
	return joinGame->m_Object1.c_str();
}

const char* ConnectionController::getLevelData(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	LevelData* levelData = static_cast<LevelData*>(m_ReceivedPackages[p_Package].get());
	return levelData->m_Object1.c_str();
}

const size_t ConnectionController::getLevelDataSize(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	LevelData* levelData = static_cast<LevelData*>(m_ReceivedPackages[p_Package].get());
	return levelData->m_Object1.size();
}

void ConnectionController::sendRacePosition(const char** p_ExtraData, unsigned int p_NumExtraData)
{
	GamePositions package;
	for(unsigned int i = 0; i < p_NumExtraData; i++)
	{
		package.m_Object1.push_back(std::string(p_ExtraData[i]));
	}
	writeData(package.getData(), (uint16_t)package.getType());
}

unsigned int ConnectionController::getNumRacePositionsData(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	GamePositions* createObjects = static_cast<GamePositions*>(m_ReceivedPackages[p_Package].get());
	return createObjects->m_Object1.size();
}

const char* ConnectionController::getRacePositionsData(Package p_Package, unsigned int p_ExtraData)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	GamePositions* createObjects = static_cast<GamePositions*>(m_ReceivedPackages[p_Package].get());
	return createObjects->m_Object1[p_ExtraData].c_str();
}

void ConnectionController::sendGameResult(const char** p_ExtraData, unsigned int p_NumExtraData)
{
	ResultData package;
	for (unsigned int i = 0; i < p_NumExtraData; ++i)
	{
		package.m_Object1.push_back(std::string(p_ExtraData[i]));
	}
	writeData(package.getData(), (uint16_t)package.getType());
}

unsigned int ConnectionController::getNumGameResultData(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	ResultData* createObjects = static_cast<ResultData*>(m_ReceivedPackages[p_Package].get());
	return createObjects->m_Object1.size();
}

const char* ConnectionController::getGameResultData(Package p_Package, unsigned int p_ExtraData)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	ResultData* createObjects = static_cast<ResultData*>(m_ReceivedPackages[p_Package].get());
	return createObjects->m_Object1[p_ExtraData].c_str();
}

void ConnectionController::sendLevelData(const char* p_Stream, size_t p_Size)
{
	LevelData package;
	package.m_Object1 = std::string(p_Stream, p_Size);
	writeData(package.getData(), (uint16_t)package.getType());
}

void ConnectionController::sendLeaveGame()
{
	LeaveGame package;
	writeData(package.getData(), (uint16_t)package.getType());
}

void ConnectionController::sendSetSpawnPosition(Vector3 p_Position)
{
	SetSpawnPosition package;
	package.m_Object1 = p_Position;
	writeData(package.getData(), (uint16_t)package.getType());
}

Vector3 ConnectionController::getSetSpawnPositionData(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	SetSpawnPosition* setSpawn = static_cast<SetSpawnPosition*>(m_ReceivedPackages[p_Package].get());
	return setSpawn->m_Object1;
}

void ConnectionController::sendThrowSpell(const char* p_SpellName, Vector3 p_StartPosition, Vector3 p_Direction)
{
	ThrowSpellData data;
	data.spellName = p_SpellName;
	data.position = p_StartPosition;
	data.direction = p_Direction;
	ThrowSpell package;
	package.m_Object1 = data;
	writeData(package.getData(), (uint16_t)package.getType());
}

const char* ConnectionController::getThrowSpellName(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	ThrowSpell* throwSpell = static_cast<ThrowSpell*>(m_ReceivedPackages[p_Package].get());
	return throwSpell->m_Object1.spellName.c_str();
}

Vector3 ConnectionController::getThrowSpellStartPosition(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	ThrowSpell* throwSpell = static_cast<ThrowSpell*>(m_ReceivedPackages[p_Package].get());
	return throwSpell->m_Object1.position;
}

Vector3 ConnectionController::getThrowSpellDirection(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	ThrowSpell* throwSpell = static_cast<ThrowSpell*>(m_ReceivedPackages[p_Package].get());
	return throwSpell->m_Object1.direction;
}

void ConnectionController::setDisconnectedCallback(IConnection::disconnectedCallback_t p_DisconnectCallback)
{
	m_Connection->setDisconnectedCallback(p_DisconnectCallback);
}

void ConnectionController::writeData(const std::string& p_Buffer, uint16_t p_ID)
{
	if (m_Connection)
	{
		m_Connection->writeData(p_Buffer, p_ID);
	}
}

void ConnectionController::savePackageCallBack(uint16_t p_ID, const std::string& p_Data)
{
	for(const PackageBase::ptr& p : m_PackagePrototypes)
	{
		if(p->getType() == (PackageType)p_ID)
		{
			PackageBase::ptr package = p->createPackage(p_Data);
			std::lock_guard<std::mutex> lock(m_ReceivedLock);
			m_ReceivedPackages.push_back(std::move(package));
			return;
		}
 	}

	std::string msg("Received unregistered package type: " + std::to_string(p_ID));
	NetworkLogger::log(NetworkLogger::Level::WARNING, msg);
}

void ConnectionController::sendStartCountdown()
{
	StartCountdown package;
	writeData(package.getData(), (uint16_t)package.getType());
}

void ConnectionController::sendDoneCountdown()
{
	DoneCountdown package;
	writeData(package.getData(), (uint16_t)package.getType());
}
