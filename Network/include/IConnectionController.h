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
	 * Send an Add Object package.
	 *
	 * @param p_Data the object data for the package.
	 */
	virtual void sendAddObject(const AddObjectData& p_Data) = 0;
	/**
	 * Get the object data from the package.
	 *
	 * @param p_Package a valid reference to a package with the AddObject type.
	 * @result a copy of the object data.
	 */
	virtual AddObjectData getAddObjectData(Package p_Package) = 0;
};