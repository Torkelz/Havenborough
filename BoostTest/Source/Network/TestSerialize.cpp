#include <boost/test/unit_test.hpp>
#include "../../../Network/Source/Packages.h"

BOOST_AUTO_TEST_SUITE(TestSerialize)

BOOST_AUTO_TEST_CASE(TestPackageSerialize)
{
	AddObjectData data = {{1.f, 2.f, 3.f}};
	AddObject package;
	package.m_Data = data;

	std::string serializedData(package.getData());

	AddObject::ptr deserializedPackage(package.createPackage(serializedData));
	AddObjectData deserializedData = ((AddObject*)deserializedPackage.get())->m_Data;

	BOOST_CHECK_EQUAL(deserializedData.m_Position[0], data.m_Position[0]);
	BOOST_CHECK_EQUAL(deserializedData.m_Position[1], data.m_Position[1]);
	BOOST_CHECK_EQUAL(deserializedData.m_Position[2], data.m_Position[2]);
}

BOOST_AUTO_TEST_SUITE_END()