#include <boost/test/unit_test.hpp>
#include "../../../Network/Source/Packages.h"

BOOST_AUTO_TEST_SUITE(TestSerialize)

BOOST_AUTO_TEST_CASE(TestPackageSerialize)
{
	CreateObjects package;

	const std::string testDescription("TestDescription");
	const uint32_t testId = 1234;
	package.m_Object1.push_back(std::make_pair(testDescription, testId));

	std::string serializedData(package.getData());

	PackageBase::ptr deserializedPackage(package.createPackage(serializedData));
	CreateObjects* rawDeserializedPackage = (CreateObjects*)deserializedPackage.get();

	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object1.size(), 1);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object1[0].first, testDescription);
	BOOST_CHECK_EQUAL(rawDeserializedPackage->m_Object1[0].second, testId);
}

BOOST_AUTO_TEST_SUITE_END()