#pragma once

#include <sstream>
#include <memory>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4244)
#include <boost/archive/binary_iarchive.hpp>
#pragma warning(pop)

class PackageBase
{
public:
	typedef std::unique_ptr<PackageBase> ptr;

	enum class Type : uint16_t
	{
		RESERVED = 0,
		PLAYER_READY,
		ADD_OBJECT,
		REMOVE_OBJECT,
		UPDATE_OBJECTS,
		GAME_RESULT,
	};

protected:
	 Type m_ID;

public:
	PackageBase(Type p_Type)
		: m_ID(p_Type)
	{}

	Type getType() const { return m_ID; };

	virtual PackageBase::ptr createPackage(const std::vector<char>& p_Data) =0;
};

class PlayerReady : public PackageBase
{
public:
	PlayerReady()
		: PackageBase(Type::PLAYER_READY)
	{}

	PackageBase::ptr createPackage(const std::vector<char>& p_Data) override
	{
		std::unique_ptr<PlayerReady> res(new PlayerReady());

		std::istringstream stream(std::string(p_Data.data(), p_Data.size()));
		boost::archive::binary_iarchive archive(stream, boost::archive::no_header);
		archive >> *res;

		return PackageBase::ptr(res.release());
	}

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
	}
};

class AddObject : public PackageBase
{
public:
	float pos[3];

public:
	AddObject()
		: PackageBase(Type::ADD_OBJECT)
	{}

	PackageBase::ptr createPackage(const std::vector<char>& p_Data) override
	{
		std::unique_ptr<AddObject> res(new AddObject());

		std::istringstream stream(std::string(p_Data.data(), p_Data.size()));
		boost::archive::binary_iarchive archive(stream, boost::archive::no_header);
		archive >> *res;

		return PackageBase::ptr(res.release());
	}

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & pos[0] & pos[1] & pos[2];
	}
};