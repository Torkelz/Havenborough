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
#include <boost/serialization/is_bitwise_serializable.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
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
 * Template for packages without arguments.
 */
template <PackageType type>
class Signal : public PackageHelper<Signal<type>>
{
public:
	/**
	 * constructor.
	 */
	Signal()
		: PackageHelper<Signal<type>>(type)
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

/**
 * Template for packages with a single simple object.
 */
template <PackageType type, class obj1>
class Package1Obj : public PackageHelper<Package1Obj<type, obj1>>
{
public:
	obj1 m_Object1;

public:
	/**
	 * constructor.
	 */
	Package1Obj()
		: PackageHelper<Package1Obj<type, obj1>>(type)
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
		ar & m_Object1;
	}
};

/**
 * Template for packages with a single simple object.
 */
template <PackageType type, class obj1, class obj2>
class Package2Obj : public PackageHelper<Package2Obj<type, obj1, obj2>>
{
public:
	obj1 m_Object1;
	obj2 m_Object2;

public:
	/**
	 * constructor.
	 */
	Package2Obj()
		: PackageHelper<Package2Obj<type, obj1, obj2>>(type)
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
		ar & m_Object1;
		ar & m_Object2;
	}
};

/**
 * Template for packages with a single simple object.
 */
template <PackageType type, class obj1, class obj2, class obj3>
class Package3Obj : public PackageHelper<Package3Obj<type, obj1, obj2, obj3>>
{
public:
	obj1 m_Object1;
	obj2 m_Object2;
	obj3 m_Object3;

public:
	/**
	 * constructor.
	 */
	Package3Obj()
		: PackageHelper<Package3Obj<type, obj1, obj2, obj3>>(type)
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
		ar & m_Object1;
		ar & m_Object2;
		ar & m_Object3;
	}
};

/**
 * A package representing that a player is ready to start a game.
 */
typedef Signal<PackageType::PLAYER_READY> PlayerReady;

/**
 * A package representing that a player has finished loading the level.
 */
typedef Signal<PackageType::DONE_LOADING> DoneLoading;

/**
 * A package representing a player leaving a game.
 */
typedef Signal<PackageType::LEAVE_GAME> LeaveGame;

/**
 * A package representing that all players are ready to start the game.
 */
typedef Signal<PackageType::START_COUNTDOWN> StartCountdown;

/**
 * A package representing that all players can start running.
 */
typedef Signal<PackageType::DONE_COUNTDOWN> DoneCountdown;

/**
 * A package for requesting available games.
 */
typedef Signal<PackageType::REQUEST_GAMES> RequestGames;

BOOST_CLASS_IMPLEMENTATION(RequestGames, boost::serialization::object_serializable)
BOOST_CLASS_TRACKING(RequestGames, boost::serialization::track_never)

/**
 * A package representing the removal of objects in the game world.
 */
typedef Package1Obj<PackageType::REMOVE_OBJECTS, std::vector<uint32_t>> RemoveObjects;

/**
 * A package representing assigning a player to an object.
 */
typedef Package1Obj<PackageType::ASSIGN_PLAYER, uint32_t> AssignPlayer;

BOOST_IS_BITWISE_SERIALIZABLE(PlayerControlData)

namespace boost
{
	namespace serialization
	{
		template <typename Archive>
		inline void serialize(Archive& ar, Vector3& m_Data, const unsigned int /*version*/)
		{
			ar & m_Data.x;
			ar & m_Data.y;
			ar & m_Data.z;
		}

		template <typename Archive>
		inline void serialize(Archive& ar, PlayerControlData& m_Data, const unsigned int /*version*/)
		{
			ar & m_Data.m_Position;
			ar & m_Data.m_Velocity;
			ar & m_Data.m_Rotation;
			ar & m_Data.m_Forward;
			ar & m_Data.m_Up;
		}
	}
}

/**
 * A package representing the number of checkpoints.
 */
typedef Package1Obj<PackageType::NUMBER_OF_CHECKPOINTS, unsigned int> NumberOfCheckpoints;

/**
 * A package representing the number of checkpoints taken by a player.
 */
typedef Package1Obj<PackageType::TAKEN_CHECKPOINTS, unsigned int> TakenCheckpoints;

/**
 * A package representing the player controlling its object.
 */
typedef Package1Obj<PackageType::PLAYER_CONTROL, PlayerControlData> PlayerControl;

/**
 * A package representing the level data.
 */
typedef Package1Obj<PackageType::LEVEL_DATA, std::string> LevelData;

/**
 * A package representing the game result.
 */
typedef Package1Obj<PackageType::RESULT_GAME, std::vector<std::string>> ResultData;

/**
 * A package representing the current checkpoint.
 */
typedef Package1Obj<PackageType::CURRENT_CHECKPOINT, Vector3> CurrentCheckpoint;

/**
 * A package representing the players positions.
 */
typedef Package1Obj<PackageType::GAME_POSITIONS, std::vector<std::string>> GamePositions;

/**
 * A package representing the addition of new objects to the game world.
 */
typedef Package1Obj<PackageType::CREATE_OBJECTS, std::vector<std::pair<std::string, uint32_t>>> CreateObjects;

/**
 * A package representing setting the respawn position of a player.
 */
typedef Package1Obj<PackageType::SET_SPAWN, Vector3> SetSpawnPosition;

struct ThrowSpellData
{
	std::string spellName;
	Vector3 position;
	Vector3 direction;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & spellName;
		ar & position;
		ar & direction;
	}
};

/**
 * A player throwing a spell.
 */
typedef Package1Obj<PackageType::THROW_SPELL, ThrowSpellData> ThrowSpell;

struct AvailableGame
{
	std::string levelName;
	uint16_t waitingPlayers;
	uint16_t maxPlayers;

	template <typename Archive>
	void  serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & levelName;
		ar & waitingPlayers;
		ar & maxPlayers;
	}
};

BOOST_CLASS_IMPLEMENTATION(AvailableGame, boost::serialization::object_serializable)
BOOST_CLASS_TRACKING(AvailableGame, boost::serialization::track_never)

BOOST_CLASS_IMPLEMENTATION(std::vector<AvailableGame>, boost::serialization::object_serializable)
BOOST_CLASS_TRACKING(std::vector<AvailableGame>, boost::serialization::track_never)

/**
 * List of available games in the server.
 */
 typedef Package1Obj<PackageType::GAME_LIST, std::vector<AvailableGame>> GameList;

BOOST_CLASS_IMPLEMENTATION(GameList, boost::serialization::object_serializable)
BOOST_CLASS_TRACKING(GameList, boost::serialization::track_never)

BOOST_IS_BITWISE_SERIALIZABLE(UpdateObjectData)

/**
 * A package representing the update of objects in the game world.
 */
 typedef Package2Obj<PackageType::UPDATE_OBJECTS, std::vector<UpdateObjectData>, std::vector<std::string>> UpdateObjects;

/**
 * A package representing one objects action in the game world.
 */
typedef Package2Obj<PackageType::OBJECT_ACTION, uint32_t, std::string> ObjectAction;

/**
 * A package representing the player joining a game.
 */
typedef Package2Obj<PackageType::JOIN_GAME, std::string, std::string> JoinGame;
