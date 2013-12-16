#include <boost/test/unit_test.hpp>
#include "../../../Client/Source/LevelBinaryLoader.h"
BOOST_AUTO_TEST_SUITE(TestBinaryLevelLoader)

class testLevelLoader : public LevelBinaryLoader
{
public:
	LevelBinaryLoader::Header testReadHeader(std::istream* p_Input)
	{
		return readHeader(p_Input);
	}
	std::vector<LevelBinaryLoader::ModelData> testReadLevelData(std::istream* p_Input)
	{
		return readLevel(p_Input);
	}
};

BOOST_AUTO_TEST_CASE(TestReadHeader)
{
	LevelBinaryLoader::Header tempHeader;
	char binHeader[] =
		"\x01\0\0\0";
	std::istringstream tempString(std::string(binHeader, binHeader + sizeof(binHeader)));
	testLevelLoader loader;
	tempHeader = loader.testReadHeader(&tempString);

	BOOST_CHECK_EQUAL(tempHeader.m_NumberOfModels, 1);
}

BOOST_AUTO_TEST_CASE(TestReadLevelData)
{
	
}

BOOST_AUTO_TEST_SUITE_END()