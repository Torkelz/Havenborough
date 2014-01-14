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
	 * @param p_Descriptions array of null-terminated strings describing the different types of objects
	 * @param p_NumDescriptions the number of descriptions in the array
	 * @param p_Instances array of object instances
	 * @param p_NumInstance the number of instances in the array
	 */
	virtual void sendCreateObjects(const char** p_Descriptions, unsigned int p_NumDescriptions, const ObjectInstance* p_Instances, unsigned int p_NumInstances) = 0;

	/**
	 * Get the number of descriptions in the package.
	 *
	 * @param p_Package a valid reference to a package with the CreateObjects type.
	 * @result the number of descriptions in the package
	 */
	virtual unsigned int getNumCreateObjectDescriptions(Package p_Package) = 0;

	/**
	 * Get a description from the package.
	 *
	 * @param p_Package a valid reference to a package with the CreateObjects type.
	 * @param p_Description the index of the description to retreive
	 * @result the description in the package
	 */
	virtual const char* getCreateObjectDescription(Package p_Package, unsigned int p_Description) = 0;

	/**
	 * Get the number of instances in the package.
	 *
	 * @param p_Package a valid reference to a package with the CreateObjects type.
	 * @result the number of instances in the package
	 */
	virtual unsigned int getNumCreateObjectInstances(Package p_Package) = 0;

	/**
	 * Get the array of descriptions in the package.
	 *
	 * @param p_Package a valid reference to a package with the CreateObjects type.
	 * @result the descriptions in the package
	 */
	virtual const ObjectInstance* getCreateObjectInstances(Package p_Package) = 0;

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
	virtual void sendRemoveObjects(const uint16_t* p_Objects, unsigned int p_NumObjects) = 0;

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
	virtual const uint16_t* getRemoveObjectRefs(Package p_Package) = 0;

	/**
	 * Send an Object Action package.
	 *
	 * @param p_ObjectId the id of the target object
	 * @param p_Action a null-terminated string describing the action
	 */
	virtual void sendObjectAction(uint16_t p_ObjectId, const char* p_Action) = 0;

	/**
	 * Get the id of the object targeted by the package.
	 *
	 * @param p_Package a valid reference to a package with the ObjectAction type.
	 * @return the id of an object
	 */
	virtual uint16_t getObjectActionId(Package p_Package) = 0;

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
	virtual void sendAssignPlayer(uint16_t p_ObjectId) = 0;

	/**
	 * @param p_Package a valid reference to a package with the AssignPlayer type.
	 * @return the id of an object
	 */
	virtual uint16_t getAssignPlayerObject(Package p_Package) = 0;

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
};
