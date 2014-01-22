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
	void testReadLight(std::istream* p_Input)
	{
		return readLevelLighting(p_Input);
	}
	void m_LevelCheckPointList(std::istream* p_Input)
	{
		return readLevelCheckPoint(p_Input);
	}
	void m_LevelCheckPointStart(std::istream* p_Input)
	{
		return readLevelCheckPoint(p_Input);
	}
};

BOOST_AUTO_TEST_CASE(TestReadHeader)
{
	LevelBinaryLoader::Header header;
	char binHeader[] =
		"\x01\0\0\0"
		"\x01\0\0\0"
		"\x05\0\0\0";
	std::istringstream tempString(std::string(binHeader, binHeader + sizeof(binHeader)));
	testLevelLoader loader;
	header = loader.testReadHeader(&tempString);

	BOOST_CHECK_EQUAL(header.m_NumberOfModels, 1);
	//BOOST_CHECK_EQUAL(header.m_NumberOfLights, 1);
	//BOOST_CHECK_EQUAL(header.m_NumberOfCheckPoints, 5);
}

BOOST_AUTO_TEST_CASE(TestReadLevelData)
{
	struct byteFloat
	{
		union
		{
			float f;
			char c[sizeof(float)];
		};
	};
	std::vector<LevelBinaryLoader::ModelData> data;
	char binData[] =
		"\x01\0\0\0"
		"\x06\0\0\0House1"
		"\x01\0\0\0"
		"\0\0pA\0\0?D\0\0\0?"
		"\x01\0\0\0"
		"\0\0\0D\0\0\0D\0\0\0D"
		"\x01\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0";
	
	std::string meshName = "House1";
	std::istringstream tempString(std::string(binData, binData + sizeof(binData)));
	testLevelLoader loader;
	data = loader.testReadLevelData(&tempString);
	byteFloat bFloat[9] = {15.0f, 764.0f, 0.5f, 512.0f, 512.0f, 512.0f, 0.0f, 0.0f, 0.0f};

	BOOST_CHECK_EQUAL_COLLECTIONS(data.at(0).m_MeshName.begin(), data.at(0).m_MeshName.end(), meshName.begin(), meshName.end() );
	BOOST_CHECK_EQUAL(data.at(0).m_Translation.at(0).x, bFloat[0].f );
	BOOST_CHECK_EQUAL(data.at(0).m_Translation.at(0).y, bFloat[1].f );
	BOOST_CHECK_EQUAL(data.at(0).m_Translation.at(0).z, bFloat[2].f );
	BOOST_CHECK_EQUAL(data.at(0).m_Rotation.at(0).x, bFloat[3].f );
	BOOST_CHECK_EQUAL(data.at(0).m_Rotation.at(0).y, bFloat[4].f );
	BOOST_CHECK_EQUAL(data.at(0).m_Rotation.at(0).z, bFloat[5].f );
	BOOST_CHECK_EQUAL(data.at(0).m_Scale.at(0).x, bFloat[6].f );
	BOOST_CHECK_EQUAL(data.at(0).m_Scale.at(0).y, bFloat[7].f );
	BOOST_CHECK_EQUAL(data.at(0).m_Scale.at(0).z, bFloat[8].f );
}

BOOST_AUTO_TEST_CASE(TestReadLevelLighting)
{

}
BOOST_AUTO_TEST_SUITE_END()