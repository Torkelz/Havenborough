#pragma once

#include <CommonTypes.h>

#include <sstream>
#include <memory>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4244)
#include <boost/archive/binary_iarchive.hpp>
#pragma warning(pop)

/**
* Add new packages in this class. Make a new package by inherit
* the PackageBase class and implement the functions that is inside it.
*/

class PackageBase
{
public:
	typedef std::unique_ptr<PackageBase> ptr;

protected:
	 PackageType m_ID;

public:
	PackageBase(PackageType p_Type)
		: m_ID(p_Type)
	{}

	PackageType getType() const { return m_ID; };

	virtual PackageBase::ptr createPackage(const std::string& p_Data) =0;

	template <typename Package>
	PackageBase::ptr createPackageImp(const std::string& p_Data)
	{
		std::unique_ptr<Package> res(new Package());

		std::istringstream stream(p_Data);
		boost::archive::binary_iarchive archive(stream, boost::archive::no_header);
		archive >> *res;

		return PackageBase::ptr(res.release());
	}
};

class PlayerReady : public PackageBase
{
public:
	PlayerReady()
		: PackageBase(PackageType::PLAYER_READY)
	{}

	PackageBase::ptr createPackage(const std::string& p_Data) override
	{
		return createPackageImp<PlayerReady>(p_Data);
	}

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
	}
};

class AddObject : public PackageBase
{
public:
	AddObjectData m_Data;

public:
	AddObject()
		: PackageBase(PackageType::ADD_OBJECT)
	{}

	PackageBase::ptr createPackage(const std::string& p_Data) override
	{
		return createPackageImp<AddObject>(p_Data);
	}

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & m_Data.m_Position[0] & m_Data.m_Position[1] & m_Data.m_Position[2];
	}
};