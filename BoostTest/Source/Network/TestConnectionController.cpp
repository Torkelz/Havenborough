#include <boost/test/unit_test.hpp>
#include "../../../Network/Source/ConnectionController.h"

BOOST_AUTO_TEST_SUITE(TestConnectionController)

class ConnectionStub : public IConnection
{
public:
	IConnection::saveDataFunction m_SaveData;

	bool isConnected() const override { return true; }
	void disconnect() override {};
	bool hasError() const override { return false; }
	void writeData(const std::string& p_Buffer, uint16_t p_ID) override
	{
		if (m_SaveData)
		{
			m_SaveData(p_ID, p_Buffer);
		}
	}
	void setSaveData(saveDataFunction p_SaveData) override
	{
		m_SaveData = p_SaveData;
	}
	void setDisconnectedCallback(disconnectedCallback_t p_DisconnectedCallback) override {}
	void startReading() override {}
};

BOOST_AUTO_TEST_CASE(TestReceivePackage)
{
	IConnection::ptr conn(new ConnectionStub);

	std::vector<PackageBase::ptr> prototypes;
	prototypes.push_back(PackageBase::ptr(new CreateObjects));

	ConnectionController controller(conn, prototypes);

	CreateObjects package;
	static const std::string testDesc = "TestDesc";
	static const uint32_t testId = 123;
	package.m_Object1.push_back(std::make_pair(testDesc, testId));

	conn->writeData(package.getData(), (uint16_t)PackageType::CREATE_OBJECTS);

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

BOOST_AUTO_TEST_CASE(TestSendUpdate)
{
	IConnection::ptr conn(new ConnectionStub);

	std::vector<PackageBase::ptr> prototypes;
	prototypes.push_back(PackageBase::ptr(new UpdateObjects));

	ConnectionController controller(conn, prototypes);

	UpdateObjectData data;
	data.m_Id = 1;
	data.m_Position = Vector3(3.f, 4.f, 5.f);
	data.m_Rotation = Vector3(6.f, 7.f, 8.f);
	data.m_RotationVelocity = Vector3(9.f, 10.f, 11.f);
	data.m_Velocity = Vector3(12.f, 13.f, 14.f);
	std::string extraData("TestExtraData");
	const char* cExtraData = extraData.c_str();

	controller.sendUpdateObjects(&data, 1, &cExtraData, 1);

	BOOST_REQUIRE_EQUAL(controller.getNumPackages(), 1);

	Package packageRef = controller.getPackage(0);
	BOOST_REQUIRE_EQUAL((uint16_t)controller.getPackageType(packageRef), (uint16_t)PackageType::UPDATE_OBJECTS);
	BOOST_REQUIRE_EQUAL(controller.getNumUpdateObjectData(packageRef), 1);
	const UpdateObjectData& recData = controller.getUpdateObjectData(packageRef)[0];

	BOOST_CHECK_EQUAL(recData.m_Id, data.m_Id);
	BOOST_CHECK_EQUAL(recData.m_Position, data.m_Position);
	BOOST_CHECK_EQUAL(recData.m_Rotation, data.m_Rotation);
	BOOST_CHECK_EQUAL(recData.m_RotationVelocity, data.m_RotationVelocity);
	BOOST_CHECK_EQUAL(recData.m_Velocity, data.m_Velocity);

	BOOST_REQUIRE_EQUAL(controller.getNumUpdateObjectExtraData(packageRef), 1);
	const char* recExtraData = controller.getUpdateObjectExtraData(packageRef, 0);

	BOOST_CHECK_EQUAL(recExtraData, extraData);
}

BOOST_AUTO_TEST_CASE(TestSendCreateObjects)
{
	IConnection::ptr conn(new ConnectionStub);

	std::vector<PackageBase::ptr> prototypes;
	prototypes.push_back(PackageBase::ptr(new CreateObjects));

	ConnectionController controller(conn, prototypes);

	ObjectInstance data;
	data.m_Id = 1;
	static const std::string testDescription("TestDescription");
	data.m_Description = testDescription.c_str();

	controller.sendCreateObjects(&data, 1);

	BOOST_REQUIRE_EQUAL(controller.getNumPackages(), 1);

	Package packageRef = controller.getPackage(0);
	BOOST_REQUIRE_EQUAL((uint16_t)controller.getPackageType(packageRef), (uint16_t)PackageType::CREATE_OBJECTS);
	BOOST_REQUIRE_EQUAL(controller.getNumCreateObjects(packageRef), 1);
	ObjectInstance recData = controller.getCreateObjectDescription(packageRef, 0);

	BOOST_CHECK_EQUAL(recData.m_Id, data.m_Id);
	BOOST_CHECK_EQUAL(recData.m_Description, testDescription);
}

BOOST_AUTO_TEST_CASE(TestSendGameResult)
{
	IConnection::ptr conn(new ConnectionStub);

	std::vector<PackageBase::ptr> prototypes;
	prototypes.push_back(PackageBase::ptr(new ResultData));

	ConnectionController controller(conn, prototypes);

	static const std::string testExtraData("TestExtraData");
	static const char* cTestExtraData = testExtraData.c_str();

	controller.sendGameResult(&cTestExtraData, 1);

	BOOST_REQUIRE_EQUAL(controller.getNumPackages(), 1);

	Package packageRef = controller.getPackage(0);
	BOOST_REQUIRE_EQUAL((uint16_t)controller.getPackageType(packageRef), (uint16_t)PackageType::RESULT_GAME);
	BOOST_REQUIRE_EQUAL(controller.getNumGameResultData(packageRef), 1);
	const char* recData = controller.getGameResultData(packageRef, 0);

	BOOST_CHECK_EQUAL(recData, testExtraData);
}

BOOST_AUTO_TEST_CASE(TestLevelData)
{
	IConnection::ptr conn(new ConnectionStub);

	std::vector<PackageBase::ptr> prototypes;
	prototypes.push_back(PackageBase::ptr(new LevelData));

	ConnectionController controller(conn, prototypes);

	static const std::string testExtraData(std::string("TestLevelStream") + '\0' + "SecondPart");

	controller.sendLevelData(testExtraData.c_str(), testExtraData.size());

	BOOST_REQUIRE_EQUAL(controller.getNumPackages(), 1);

	Package packageRef = controller.getPackage(0);
	BOOST_REQUIRE_EQUAL((uint16_t)controller.getPackageType(packageRef), (uint16_t)PackageType::LEVEL_DATA);
	BOOST_REQUIRE_EQUAL(controller.getLevelDataSize(packageRef), testExtraData.size());
	const char* recData = controller.getLevelData(packageRef);

	BOOST_CHECK_EQUAL(std::string(recData, testExtraData.size()), testExtraData);
}

BOOST_AUTO_TEST_SUITE_END()