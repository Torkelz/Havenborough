#include <boost/test/unit_test.hpp>
#include "../../../Network/Source/Packages.h"

BOOST_AUTO_TEST_SUITE(TestSerialize)

BOOST_AUTO_TEST_CASE(TestPackageSerialize)
{
	CreateObjects package;

	std::string testDescription("TestDescription");
	package.m_Object1.push_back(testDescription);
	ObjectInstance inst;
	inst.m_DescriptionIdx = 0;
	inst.m_Id = 1234;
	inst.m_Position.x = 3.f;
	inst.m_Position.y = 4.f;
	inst.m_Position.z = 5.f;
	inst.m_Rotation.x = 7.f;
	inst.m_Rotation.y = 8.f;
	inst.m_Rotation.z = 9.f;
	package.m_Object2.push_back(inst);

	std::string serializedData(package.getData());

	PackageBase::ptr deserializedPackage(package.createPackage(serializedData));
	CreateObjects* rawDeserializedPackage = (CreateObjects*)deserializedPackage.get();

	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object1.size(), 1);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object1[0], testDescription);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2.size(), 1);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_DescriptionIdx, inst.m_DescriptionIdx);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Id, inst.m_Id);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Position.x, inst.m_Position.x);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Position.y, inst.m_Position.y);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Position.z, inst.m_Position.z);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Rotation.x, inst.m_Rotation.x);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Rotation.y, inst.m_Rotation.y);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Rotation.z, inst.m_Rotation.z);
}

BOOST_AUTO_TEST_SUITE_END()