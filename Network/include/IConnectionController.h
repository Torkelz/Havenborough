/**
 * File comment.
 */
#pragma once

#include <CommonTypes.h>

/**
 * Interface for using a network connnection.
 *
 * Supports sending and polling for received packages.
 */
class IConnectionController
{
public:
	/**
	 *
	 */
	virtual ~IConnectionController() {};

	/**
	 * Check if the controlled connection is connected.
	 *
	 * @return true if connected, otherwise false.
	 */
	virtual bool isConnected() const = 0;

	/**
	 * Check if an error has occurred.
	 *
	 * @return true if an error has occurred, otherwise false.
	 */
	virtual bool hasError() const = 0;

	/**
	 * Get the number of packages currently stored. Use with caution,
	 * as it is updated asynchronously.
	 *
	 * @return the number of packages waiting.
	 */
	virtual unsigned int getNumPackages() = 0;
	/**
	 * Get a reference to one of the received packages.
	 *
	 * The package is not removed.
	 *
	 * @param p_Index the index of the package to get a reference to.
	 *			Using an invalid index is undefined behaviour.
	 * @return opaque reference to a package if existing.
	 */
	virtual Package getPackage(unsigned int p_Index) = 0;
	/**
	 * Clear a number of packages from the front of the queue of received packages.
	 *
	 * This invalidates any retreived Package references.
	 *
	 * @param p_NumPackages the number of packages to remove.
	 *			Should not be larger than the number of stored packages.
	 */
	virtual void clearPackages(unsigned int p_NumPackages) = 0;

	/**
	 * Get the type of a package.
	 *
	 * @param p_Package a valid reference to a package.
	 * @return the type of the package.
	 */
	virtual PackageType getPackageType(Package p_Package) = 0;

	/**
	 * Send a Create Objects package.
	 *
	 * @param p_Instances array of object instances, containing null-terminated descriptions and actor ids
	 * @param p_NumInstance the number of instances in the array
	 */
	virtual void sendCreateObjects(const ObjectInstance* p_Instances, unsigned int p_NumInstances) = 0;

	/**
	 * Get the number of objects in the package.
	 *
	 * @param p_Package a valid reference to a package with the CreateObjects type.
	 * @result the number of objects in the package
	 */
	virtual unsigned int getNumCreateObjects(Package p_Package) = 0;

	/**
	 * Get a description from the package.
	 *
	 * @param p_Package a valid reference to a package with the CreateObjects type.
	 * @param p_Description the index of the description to retreive
	 * @result the description in the package
	 */
	virtual ObjectInstance getCreateObjectDescription(Package p_Package, unsigned int p_Description) = 0;

	/**
	 * Send an Update Objects package.
	 *
	 * @param p_ObjectData array of object updates to send
	 * @param p_NumObjects the number of object updates in the array
	 * @param p_ExtraData array of null-terminated string with extra data
	 * @param p_NumExtraData the number of extra data strings
	 */
	virtual void sendUpdateObjects(const UpdateObjectData* p_ObjectData, unsigned int p_NumObjects, const char** p_ExtraData, unsigned int p_NumExtraData) = 0;

	/**
	 * Get the number of object updates in the package.
	 *
	 * @param p_Package a valid reference to a package with the UpdateObjects type.
	 * @result the number of object updates in the package
	 */
	virtual unsigned int getNumUpdateObjectData(Package p_Package) = 0;

	/**
	 * Get the array of object updates in the package.
	 *
	 * @param p_Package a valid reference to a package with the UpdateObjects type.
	 * @return an array of object updates
	 */
	virtual const UpdateObjectData* getUpdateObjectData(Package p_Package) = 0;

	/**
	 * Get the number of extra data in the package.
	 *
	 * @param p_Package a valid reference to a package with the UpdateObjects type.
	 * @return the number of extra data strings in the package
	 */
	virtual unsigned int getNumUpdateObjectExtraData(Package p_Package) = 0;

	/**
	 * Get an extra data string from the package.
	 *
	 * @param p_Package a valid reference to a package with the UpdateObjects type.
	 * @param p_ExtraData the index of the extra data to retreive
	 * @return a null-terminated string containing any extra data
	 */
	virtual const char* getUpdateObjectExtraData(Package p_Package, unsigned int p_ExtraData) = 0;

	/**
	 * Send a Remove Objects package.
	 *
	 * @param p_Objects array of objects to be removed
	 * @param p_NumObjects the number of objects in the array
	 */
	virtual void sendRemoveObjects(const uint32_t* p_Objects, unsigned int p_NumObjects) = 0;

	/**
	 * Get the number of objects in the package.
	 *
	 * @param p_Package a valid reference to a package with the RemoveObjects type.
	 * @return the number of objects in the package
	 */
	virtual unsigned int getNumRemoveObjectRefs(Package p_Package) = 0;

	/**
	 * Get the array of objects in the package.
	 *
	 * @param p_Package a valid reference to a package with the RemoveObjects type.
	 * @return an array of object references to objects to remove
	 */
	virtual const uint32_t* getRemoveObjectRefs(Package p_Package) = 0;

	/**
	 * Send an Object Action package.
	 *
	 * @param p_ObjectId the id of the target object
	 * @param p_Action a null-terminated string describing the action
	 */
	virtual void sendObjectAction(uint32_t p_ObjectId, const char* p_Action) = 0;

	/**
	 * Get the id of the object targeted by the package.
	 *
	 * @param p_Package a valid reference to a package with the ObjectAction type.
	 * @return the id of an object
	 */
	virtual uint32_t getObjectActionId(Package p_Package) = 0;

	/**
	 * Get the action from the package.
	 *
	 * @param p_Package a valid reference to a package with the ObjectAction type.
	 * @return a null-terminated string describing the action
	 */
	virtual const char* getObjectActionAction(Package p_Package) = 0;

	/**
	 * Send an Assign Player package, assigning a player to an object.
	 *
	 * @param p_ObjectId the id of the object to assign the player to
	 */
	virtual void sendAssignPlayer(uint32_t p_ObjectId) = 0;

	/**
	 * @param p_Package a valid reference to a package with the AssignPlayer type.
	 * @return the id of an object
	 */
	virtual uint32_t getAssignPlayerObject(Package p_Package) = 0;

	/**
	 * Send a Player Control package, controlling the players object.
	 *
	 * @param p_Data the data to send
	 */
	virtual void sendPlayerControl(PlayerControlData p_Data) = 0;

	/**
	 * Get the player control data from a package
	 *
	 * @param p_Package a valid reference to a package with the PlayerControl type.
	 * @return the player control data
	 */
	virtual PlayerControlData getPlayerControlData(Package p_Package) = 0;

	/**
	 * Send a Done Loading package singaling that the client has finished loading the level.
	 */
	virtual void sendDoneLoading() = 0;

	/**
	 * Send a Join Game package for a player joining a server game.
	 *
	 * @param p_Game the name of the game to join
	 * @param p_Username the name the user wants to use
	 */
	virtual void sendJoinGame(const char* p_Game, const char* p_Username) = 0;

	/**
	 * Get the name of the game the player tries to join.
	 *
	 * @param p_Package a valid reference to a package with the JoinGame type.
	 * @return the name of the game from the package
	 */
	virtual const char* getJoinGameName(Package p_Package) = 0;

	/**
	 * Get the name of the player trying to join.
	 *
	 * @param p_Package a valid reference to a package with the JoinGame type.
	 * @return the name of the player from the package
	 */
	virtual const char* getJoinGameUsername(Package p_Package) = 0;

	/**
	 * Get the size of the binary stream.
	 *
	 * @return size_t pointer with the size of the stream.
	 */
	virtual const size_t getLevelDataSize(Package p_Package) = 0;

	/**
	 * Get the game level information from server,
	 * use getLevelDataLenght() to prevent the file to stop read at a NULL value.
	 *
	 * @param p_Package a valid reference to a package with the LevelData type.
	 * @return a binary stream with the information.
	 */
	virtual const char* getLevelData(Package p_Package) = 0;

	/**
	 * Send information about the level. 
	 *
	 * @param p_Stream is a binary stream with level information.
	 * @param p_Size is the size och the binary stream in bytes.
	 */
	virtual void sendLevelData(const char* p_Stream, size_t p_Size) = 0;

	virtual void sendRacePosition(const char** p_ExtraData, unsigned int p_NumExtraData) = 0;

	virtual unsigned int getNumRacePositionsData(Package p_Package) = 0;

	virtual const char* getRacePositionsData(Package p_Package, unsigned int p_ExtraData) = 0;

	/**
	 * Send information about who won and what place you got.
	 *
	 * @param p_ExtraData is a xml code package that contains information about the game result.
	 * @param p_NumExtraData dont know yet.
	 */
	virtual void sendGameResult(const char** p_ExtraData, unsigned int p_NumExtraData) = 0;
	
	virtual unsigned int getNumGameResultData(Package p_Package) = 0;
	
	virtual const char* getGameResultData(Package p_Package, unsigned int p_ExtraData) = 0;

	/**
	 * Send a Leave Game package for a player leaving a server game.
	 */
	virtual void sendLeaveGame() = 0;
	
	/**
	 * Send a package assigning a new spawn position to the player.
	 *
	 * @param p_Position the new spawn position in world space, cm.
	 */
	virtual void sendSetSpawnPosition(Vector3 p_Position) = 0;

	/**
	 * Get the position from a set spawn position package.
	 *
	 * @param p_Package a valid reference to a package with the SetSpawnPosition type.
	 * @return the new spawn position
	 */
	virtual Vector3 getSetSpawnPositionData(Package p_Package) = 0;

	/**
	 * Send a package to throw a spell.
	 *
	 * @param p_SpellName the spell identifier for the spell
	 * @param p_StartPosition the position the spell starts at
	 * @param p_Direction the direction the spell is thrown in
	 */
	virtual void sendThrowSpell(const char* p_SpellName, Vector3 p_StartPosition, Vector3 p_Direction) = 0;

	virtual const char* getThrowSpellName(Package p_Package) = 0;
	virtual Vector3 getThrowSpellStartPosition(Package p_Package) = 0;
	virtual Vector3 getThrowSpellDirection(Package p_Package) = 0;

	/**
	 * Send a package to start countdown on clients.
	 */
	virtual void sendStartCountdown() = 0;

	/**
	 * Send a package to release players from countdown freeze.
	 */
	virtual void sendDoneCountdown() = 0;

	/**
	 * Send a package to request avilable games.
	 */
	virtual void sendRequestGames() = 0;

	/**
	 * Send a package with a list of available games.
	 *
	 * @param p_Games array of available games
	 * @param p_NumGames number of games in list
	 */
	virtual void sendGameList(const AvailableGameData* p_Games, unsigned int p_NumGames) = 0;

	/**
	 * Get the number of games in the package.
	 *
	 * @param p_Package a valid reference to a package with the GameList type.
	 * @return the number of games in the package
	 */
	virtual unsigned int getNumGameListGames(Package p_Package) = 0;

	/**
	 * Gets one of the available games in the package.
	 *
	 * @param p_Package a valid reference to a package with the GameList type.
	 * @param p_GameIdx the index of the game to retreive
	 * @return an array of available games on the server
	 */
	virtual AvailableGameData getGameListGame(Package p_Package, unsigned int p_GameIdx) = 0;
};
