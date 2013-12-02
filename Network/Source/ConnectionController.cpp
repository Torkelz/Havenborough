#include "ConnectionController.h"

#include <boost/archive/binary_oarchive.hpp>

ConnectionController::ConnectionController(Connection::ptr p_Connection, const std::vector<PackageBase::ptr>& p_Prototypes)
	:	m_PackagePrototypes(p_Prototypes),
		m_Connection(std::move(p_Connection))
{
	m_Connection->setSaveData(std::bind(&ConnectionController::savePackageCallBack, this, std::placeholders::_1, std::placeholders::_2));
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

void ConnectionController::sendAddObject(const AddObjectData& p_Data)
{
	AddObject package;
	package.m_Data = p_Data;

	std::ostringstream stream;
	boost::archive::binary_oarchive archive(stream, boost::archive::no_header);
	archive << package;

	writeData(stream.str(), (uint16_t)package.getType());
}

AddObjectData ConnectionController::getAddObjectData(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	AddObject* addObject = static_cast<AddObject*>(m_ReceivedPackages[p_Package].get());
	return addObject->m_Data;
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
	for(const std::unique_ptr<PackageBase>& p : m_PackagePrototypes)
	{
		if(p->getType() == (PackageType)p_ID)
		{
			PackageBase::ptr package = p->createPackage(p_Data);
			std::lock_guard<std::mutex> lock(m_ReceivedLock);
			m_ReceivedPackages.push_back(std::move(package));
			return;
		}
 	}

	std::cout << "Received unregistered package type: " << p_ID << std::endl;
}
