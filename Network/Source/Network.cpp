#include "Network.h"

#include <boost/archive/binary_oarchive.hpp>

Network::Network()
{
	m_Handler = NULL;

	registerPackages();
}

Network::~Network()
{

}

INetwork *INetwork::createNetwork()
{
	return new Network();
}

void INetwork::deleteNetwork(INetwork * p_Network)
{
	delete p_Network;
}

void Network::createServer(unsigned short p_Port)
{
	m_Handler = new NetworkHandler(p_Port);
	m_Handler->setSaveData(std::bind(&Network::savePackageCallBack, this, std::placeholders::_1, std::placeholders::_2));
	m_Handler->startServer();
}

void Network::turnOfServer()
{
	m_Handler->stopServer();
}

void Network::createClient(unsigned short p_Port)
{
	m_Handler = new NetworkHandler();
	m_Handler->setSaveData(std::bind(&Network::savePackageCallBack, this, std::placeholders::_1, std::placeholders::_2));
}

void Network::connectToServer(const char* p_URL, unsigned short p_Port, actionDoneCallback p_DoneHandler, void* p_UserData)
{
	m_Handler->connectToServer(p_URL, p_Port, p_DoneHandler, p_UserData);
}

bool Network::isConnected() const
{
	return m_Handler->isConnected();
}

bool Network::hasError() const
{
	return m_Handler->hasError();
}

boost::asio::io_service& Network::getServerService()
{
	return m_Handler->getServerService();
}

void Network::sendAddObject(const AddObjectData& p_Data)
{
	AddObject package;
	package.m_Data = p_Data;

	std::ostringstream stream;
	boost::archive::binary_oarchive archive(stream, boost::archive::no_header);
	archive << package;

	m_Handler->writeData(stream.str(), (uint16_t)package.getType());
}

unsigned int Network::getNumPackages()
{
	return m_ReceivedPackages.size();
}

INetwork::Package Network::getPackage(unsigned int p_Index)
{
	return p_Index;
}

PackageType Network::getPackageType(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	if (m_ReceivedPackages.size() > p_Package)
		return m_ReceivedPackages[p_Package]->getType();
	else
		return PackageType::RESERVED;
}

void Network::clearPackages(unsigned int p_NumPackages)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	m_ReceivedPackages.erase(m_ReceivedPackages.begin(), m_ReceivedPackages.begin() + p_NumPackages);
}

AddObjectData Network::getAddObjectData(Package p_Package)
{
	std::lock_guard<std::mutex> lock(m_ReceivedLock);
	AddObject* addObject = static_cast<AddObject*>(m_ReceivedPackages[p_Package].get());
	return addObject->m_Data;
}

void Network::savePackageCallBack(uint16_t p_ID, const std::string& p_Data)
{
	for(std::unique_ptr<PackageBase>& p : m_PackagePrototypes)
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

void Network::registerPackages()
{
	m_PackagePrototypes.push_back(PackageBase::ptr(new AddObject));
}
