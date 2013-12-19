#include <boost/test/unit_test.hpp>
#include "../../../BinaryConverter/Source/LevelConverter.h"
BOOST_AUTO_TEST_SUITE(TestLevelConverter)

class testConv : public LevelConverter
{
public:
	void testCreateHeader(std::ostream* p_Output)
	{
		createHeader(p_Output);
	}

	void testCreateLevel(std::ostream* p_Output)
	{
		createLevel(p_Output);
	}
};

BOOST_AUTO_TEST_CASE(TestCreateHeader)
{
	testConv conv;

	static const char binHeader[] = "\x01\0\0\0";

	LevelLoader::LevelHeader header;
	header.m_NumberOfModels = 1;
	conv.setLevelHead(header);

	std::ostringstream output;
	conv.testCreateHeader(&output);

	std::string resHeader = output.str();

	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin(), resHeader.end(), binHeader, binHeader + sizeof(binHeader) - 1);
}

BOOST_AUTO_TEST_CASE(TestCreateLevel)
{
	testConv conv;
	struct byteFloat
	{
		union
		{
			float f;
			char c[sizeof(float)];
		};
	};
	byteFloat float3Trans[3];
	static const char binHeader[] = 
		"\x01\0\0\0"
		"\x04\0\0\0Test"
		"\x01\0\0\0";

	std::vector<LevelLoader::LevelStruct> level;
	level.resize(1);
	level.at(0).m_MeshName = "Test";
	float3Trans[0].f = 0.75f; float3Trans[1].f = 1204.0f; float3Trans[2].f = 0.999f;
	level.at(0).m_Translation = DirectX::XMFLOAT3(float3Trans[0].f, float3Trans[1].f, float3Trans[2].f);
	byteFloat float3Rot[3];
	float3Rot[0].f = 75.f; float3Rot[1].f = 14.0f; float3Rot[2].f = 0.999f;
	level.at(0).m_Rotation = DirectX::XMFLOAT3(float3Rot[0].f, float3Rot[1].f, float3Rot[2].f);
	byteFloat float3Scal[3];
	float3Scal[0].f = 7.5f; float3Scal[1].f = 1.40f; float3Scal[2].f = 9.99f;
	level.at(0).m_Scale = DirectX::XMFLOAT3(float3Scal[0].f, float3Scal[1].f, float3Scal[2].f);

	conv.setLevelModelList(&level);

	std::ostringstream output;
	conv.testCreateLevel(&output);

	std::string resHeader = output.str();
	char size[] = "\x01\0\0\0";
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin(), resHeader.begin() + 16, binHeader, binHeader + sizeof(binHeader) - 1);
	//BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 16, resHeader.begin() + 20, float3Trans[0].c , float3Trans[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 20, resHeader.begin() + 24, float3Trans[1].c , float3Trans[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 24, resHeader.begin() + 28, float3Trans[2].c , float3Trans[2].c + sizeof(float));
	//BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 28, resHeader.begin() + 32, size , size + sizeof(int));
	//BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 32, resHeader.begin() + 36, float3Rot[0].c , float3Rot[0].c + sizeof(float));
	//BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 36, resHeader.begin() + 40, float3Rot[1].c , float3Rot[1].c + sizeof(float));
	//BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 40, resHeader.begin() + 44, float3Rot[2].c , float3Rot[2].c + sizeof(float));
	//BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 44, resHeader.begin() + 48, size , size + sizeof(int));
	//BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 48, resHeader.begin() + 52, float3Scal[0].c , float3Scal[0].c + sizeof(float));
	//BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 52, resHeader.begin() + 56, float3Scal[1].c , float3Scal[1].c + sizeof(float));
	//BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 56, resHeader.begin() + 60, float3Scal[2].c , float3Scal[2].c + sizeof(float));
}

BOOST_AUTO_TEST_SUITE_END()