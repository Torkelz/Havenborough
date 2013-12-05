#include <boost/test/unit_test.hpp>
#include "../../../Network/Source/ConnectionController.h"

BOOST_AUTO_TEST_SUITE(TestConnectionController)

class ConnectionStub : public Connection
{
public:
	ConnectionStub(boost::asio::io_service& p_IO_Service)
		: Connection(boost::asio::ip::tcp::socket(p_IO_Service))
	{}

	void receiveData(uint16_t p_ID, const std::string& p_Data)
	{
		if (m_SaveData)
		{
			m_SaveData(p_ID, p_Data);
		}
	}
};

BOOST_AUTO_TEST_CASE(TestReceivePackage)
{
	boost::asio::io_service ioService;
	ConnectionStub* rawConnectionStub = new ConnectionStub(ioService);
	std::unique_ptr<Connection> conn(rawConnectionStub);

	std::vector<PackageBase::ptr> prototypes;
	prototypes.push_back(PackageBase::ptr(new AddObject));

	ConnectionController controller(std::move(conn), prototypes);

	AddObject package;
	package.m_Data.m_Position[0] = 4.f;
	package.m_Data.m_Position[1] = 3.f;
	package.m_Data.m_Position[2] = 7.f;

	rawConnectionStub->receiveData((uint16_t)PackageType::ADD_OBJECT, package.getData());

	BOOST_REQUIRE_EQUAL(controller.getNumPackages(), 1);
	
	Package packageRef = controller.getPackage(0);

	BOOST_REQUIRE_EQUAL((uint16_t)controller.getPackageType(packageRef), (uint16_t)PackageType::ADD_OBJECT);

	AddObjectData objectData = controller.getAddObjectData(packageRef);

	BOOST_CHECK_EQUAL_COLLECTIONS(objectData.m_Position, objectData.m_Position + 3, package.m_Data.m_Position, package.m_Data.m_Position + 3);

	controller.clearPackages(1);

	BOOST_CHECK_EQUAL(controller.getNumPackages(), 0);
}

BOOST_AUTO_TEST_SUITE_END()