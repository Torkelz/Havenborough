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
	inst.m_Position[0] = 3.f;
	inst.m_Position[1] = 4.f;
	inst.m_Position[2] = 5.f;
	inst.m_Rotation[0] = 7.f;
	inst.m_Rotation[1] = 8.f;
	inst.m_Rotation[2] = 9.f;
	package.m_Object2.push_back(inst);

	std::string serializedData(package.getData());

	PackageBase::ptr deserializedPackage(package.createPackage(serializedData));
	CreateObjects* rawDeserializedPackage = (CreateObjects*)deserializedPackage.get();

	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object1.size(), 1);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object1[0], testDescription);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2.size(), 1);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_DescriptionIdx, inst.m_DescriptionIdx);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Id, inst.m_Id);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Position[0], inst.m_Position[0]);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Position[1], inst.m_Position[1]);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Position[2], inst.m_Position[2]);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Rotation[0], inst.m_Rotation[0]);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Rotation[1], inst.m_Rotation[1]);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object2[0].m_Rotation[2], inst.m_Rotation[2]);
}

BOOST_AUTO_TEST_SUITE_END()