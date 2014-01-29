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
	Connection::ptr conn(rawConnectionStub);

	std::vector<PackageBase::ptr> prototypes;
	prototypes.push_back(PackageBase::ptr(new CreateObjects));

	ConnectionController controller(std::move(conn), prototypes);

	CreateObjects package;
	static const std::string testDesc = "TestDesc";
	static const uint32_t testId = 123;
	package.m_Object1.push_back(std::make_pair(testDesc, testId));

	rawConnectionStub->receiveData((uint16_t)PackageType::CREATE_OBJECTS, package.getData());

	BOOST_REQUIRE_EQUAL(controller.getNumPackages(), 1);
	
	Package packageRef = controller.getPackage(0);

	BOOST_REQUIRE_EQUAL((uint16_t)controller.getPackageType(packageRef), (uint16_t)PackageType::CREATE_OBJECTS);

	BOOST_REQUIRE_EQUAL(controller.getNumCreateObjects(packageRef), 1);
	ObjectInstance objectInstance = controller.getCreateObjectDescription(packageRef, 0);

	BOOST_CHECK_EQUAL(objectInstance.m_Description, testDesc);
	BOOST_CHECK_EQUAL(objectInstance.m_Id, testId);

	controller.clearPackages(1);

	BOOST_CHECK_EQUAL(controller.getNumPackages(), 0);
}

BOOST_AUTO_TEST_SUITE_END()