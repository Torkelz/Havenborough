/**
* Add new packages in this file. Make a new package by inheriting
* the PackageBase class and implement the functions that is inside it.
 */

#pragma once

#include <CommonTypes.h>

#include <sstream>
#include <memory>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4244)
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/is_bitwise_serializable.hpp>
#pragma warning(pop)

/**
 * Abstract base class for packages.
 */
class PackageBase
{
public:
	/**
	 * PackageBase pointer type.
	 */
	typedef std::unique_ptr<PackageBase> ptr;

protected:
	/**
	 * Unique id per package type.
	 */
	PackageType m_ID;

	/**
	 * Create a package from a byte stream.
	 *
	 * @param <Package> the package type to create.
	 * @param p_Data a serialized package of the target type.
	 * @return a new package of the target type.
	 */
	template <typename Package>
	PackageBase::ptr createPackageImp(const std::string& p_Data)
	{
		std::unique_ptr<Package> res(new Package());

		std::istringstream stream(p_Data);
		boost::archive::binary_iarchive archive(stream, boost::archive::no_header);
		archive >> *res;

		return PackageBase::ptr(res.release());
	}

	/**
	 * Create a byte stream from a package.
	 *
	 * @param <Package> the package type to serialize.
	 * @param p_Package the package to serialize.
	 * @return a byte stream from the package.
	 */
	template <typename Package>
	std::string getDataImp(const Package& p_Package)
	{
		std::ostringstream ostream;
		boost::archive::binary_oarchive archive(ostream, boost::archive::no_header);
		archive << p_Package;

		return ostream.str();
	}

public:
	/**
	 * Constructor setting the package type.
	 *
	 * @param p_Type a unique identifier for the package.
	 */
	PackageBase(PackageType p_Type)
		: m_ID(p_Type)
	{}

	virtual ~PackageBase() {};

	/**
	 * Get the type of the package.
	 */
	PackageType getType() const { return m_ID; };

	/**
	 * Create a package of the same type from a byte stream.
	 *
	 * @param p_Data a serialized package data stream.
	 * @return a new deserialized package.
	 */
	virtual PackageBase::ptr createPackage(const std::string& p_Data) = 0;

	/**
	 * Get the serialized data from the package.
	 *
	 * @return the serialized package.
	 */
	virtual std::string getData() = 0;
};

/**
 * Helper class to simplify package creation.
 *
 * Provides createPackage and getData.
 *
 * @param <Package> the target subclass to provide methods to.
 */
template <typename Package>
class PackageHelper : public PackageBase
{
public:
	PackageHelper(PackageType p_Type)
		: PackageBase(p_Type)
	{}

	PackageBase::ptr createPackage(const std::string& p_Data) override
	{
		return createPackageImp<Package>(p_Data);
	}

	std::string getData() override
	{
		return getDataImp<Package>(*(Package*)this);
	}
};

/**
 * A package representing that a player is ready to start a game.
 */
class PlayerReady : public PackageHelper<PlayerReady>
{
public:
	/**
	 * constructor.
	 */
	PlayerReady()
		: PackageHelper<PlayerReady>(PackageType::PLAYER_READY)
	{}

	/**
	 * Serialize the package to or from an archive.
	 *
	 * @param <Archive> the archive type to serialize with.
	 *			Can be either input or output archives.
	 * @param ar the archive used.
	 * @param version the desired or given archive version. Ignored.
	 */
	template <typename Archive>
	void serialize(Archive& /*ar*/, const unsigned int /*version*/)
	{
	}
};

BOOST_IS_BITWISE_SERIALIZABLE(ObjectInstance)

/**
 * A package representing the addition of new objects to the game world.
 */
class CreateObjects : public PackageHelper<CreateObjects>
{
public:
	std::vector<std::string> m_Descriptions;
	std::vector<ObjectInstance> m_Instances;

public:
	/**
	 * constructor.
	 */
	CreateObjects()
		: PackageHelper<CreateObjects>(PackageType::CREATE_OBJECTS)
	{}

	/**
	 * Serialize the package to or from an archive.
	 *
	 * @param <Archive> the archive type to serialize with.
	 *			Can be either input or output archives.
	 * @param ar the archive used.
	 * @param version the desired or given archive version. Ignored.
	 */
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & m_Descriptions;
		ar & m_Instances;
	}
};

BOOST_IS_BITWISE_SERIALIZABLE(UpdateObjectData)

/**
 * A package representing the update of objects in the game world.
 */
class UpdateObjects : public PackageHelper<UpdateObjects>
{
public:
	std::vector<UpdateObjectData> m_ObjectUpdates;
	std::vector<std::string> m_Extra;

public:
	/**
	 * constructor.
	 */
	UpdateObjects()
		: PackageHelper<UpdateObjects>(PackageType::UPDATE_OBJECTS)
	{}

	/**
	 * Serialize the package to or from an archive.
	 *
	 * @param <Archive> the archive type to serialize with.
	 *			Can be either input or output archives.
	 * @param ar the archive used.
	 * @param version the desired or given archive version. Ignored.
	 */
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & m_ObjectUpdates;
		ar & m_Extra;
	}
};
