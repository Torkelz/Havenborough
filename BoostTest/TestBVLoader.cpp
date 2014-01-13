#include <boost/test/unit_test.hpp>
#include "../../../Havenborough/Physics/Source/BVLoader.h"

BOOST_AUTO_TEST_SUITE(TestBVLoader)

//class testBVLoader : public BVLoader
//{
//public:
//	BVLoader::Header testBVHeader(std::istream* p_Input)
//	{
//		return readHeader(p_Input);
//	}
//
//	std::vector<BVLoader::BoundingVolume> testBoundingVolume(int p_NumberOfVertex, std::istream* p_Input)
//	{
//		return readBoundingVolume(p_NumberOfVertex, p_Input);
//	}
//};
//
//BOOST_AUTO_TEST_CASE(TestReadBVHeader)
//{
//	char teststring[] = {
//		"\x04\0\0\0test"
//		"\0\0\0\0"
//		"\x08\0\0\0"
//		"\x01\0\0\0"
//		"\0\0\0\0"
//		"\0\0\0\0"};
//	BVLoader::Header head;
//	std::istringstream stream(std::string(teststring, teststring + sizeof(teststring)));
//	testBVLoader BVloader;
//
//	head = BVloader.testBVHeader(&stream);
//
//	BOOST_CHECK_EQUAL(head.m_modelName, "test");
//	BOOST_CHECK_EQUAL(head.m_numMaterial, 0);
//	BOOST_CHECK_EQUAL(head.m_numVertex, 8);
//	BOOST_CHECK_EQUAL(head.m_numMaterialBuffer, 1);
//	BOOST_CHECK_EQUAL(head.m_numJoints, 0);
//	BOOST_CHECK_EQUAL(head.m_numFrames, 0);	
//}
//
//BOOST_AUTO_TEST_CASE(TestReadBoundingVolume)
//{
//	char teststring[] = {
//		"\0\0\0?\0\0\0D\0\0\0\0"};
//	std::vector<BVLoader::BoundingVolume> vertex;
//	std::istringstream stream(std::string(teststring, teststring + sizeof(teststring)));
//	testBVLoader BVloader;
//
//	vertex = BVloader.testBoundingVolume(1, &stream);
//	BOOST_CHECK_EQUAL(vertex.at(0).m_Postition.x, 0.5f);
//	BOOST_CHECK_EQUAL(vertex.at(0).m_Postition.y, 512.0f);
//	BOOST_CHECK_EQUAL(vertex.at(0).m_Postition.z, 0);
//}


BOOST_AUTO_TEST_SUITE_END()