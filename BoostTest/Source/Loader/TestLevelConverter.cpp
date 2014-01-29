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
	
	void testCreateLight(std::ostream* p_Output)
	{
		createLighting(p_Output);
	}

	void testCreateCheckPoints(std::ostream* p_Output)
	{
		createCheckPoints(p_Output);
	}
};

BOOST_AUTO_TEST_CASE(TestCreateHeader)
{
	testConv conv;

	static const char binHeader[] = "\x01\0\0\0"
		"\x05\0\0\0"
		"\x03\0\0\0";

	LevelLoader::LevelHeader header;
	header.m_NumberOfModels = 1;
	header.m_NumberOfLights = 5;
	header.m_NumberOfCheckPoints = 3;
	conv.setLevelHead(header);

	std::ostringstream output;
	conv.testCreateHeader(&output);

	std::string resHeader = output.str();

	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin(), resHeader.begin()+4, binHeader, binHeader + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin()+4, resHeader.begin()+8, binHeader+4, binHeader+4 + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin()+8, resHeader.end(), binHeader+8, binHeader+8 + sizeof(int));
}

BOOST_AUTO_TEST_CASE(TestCreateLighting)
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
	struct byteInt
	{
		union
		{
			int i;
			char c[sizeof(int)];
		};
	};

	std::vector<std::pair<LevelLoader::LightData, LevelLoader::DirectionalLight>> directionalLight;
	directionalLight.resize(1);
	byteFloat float3Trans[3];
	float3Trans[0].f = 0.75f; float3Trans[1].f = 1204.0f; float3Trans[2].f = 0.999f;
	directionalLight.at(0).first.m_Translation = DirectX::XMFLOAT3(float3Trans[0].f, float3Trans[1].f, float3Trans[2].f);
	byteFloat float3Colors[3];
	float3Colors[0].f = 0.75f; float3Colors[1].f = 1.0f; float3Colors[2].f = 0.999f;
	directionalLight.at(0).first.m_Color = DirectX::XMFLOAT3(float3Colors[0].f, float3Colors[1].f, float3Colors[2].f);
	byteInt intTypeD; 
	intTypeD.i = 0;
	directionalLight.at(0).first.m_Type = intTypeD.i;
	byteFloat float3Direction[3];
	float3Direction[0].f = -0.159335f; float3Direction[1].f = -0.273098f; float3Direction[2].f = -0.948699f;  
	directionalLight.at(0).second.m_Direction = DirectX::XMFLOAT3(float3Direction[0].f, float3Direction[1].f, float3Direction[2].f);
	byteFloat floatIntensity;
	floatIntensity.f = 1.f;
	directionalLight.at(0).second.m_Intensity = floatIntensity.f;

	conv.setLevelDirectionalLightList(&directionalLight);

	std::vector<std::pair<LevelLoader::LightData, LevelLoader::PointLight>> pointLight;
	pointLight.resize(1);
	float3Trans[0].f = 0.75f; float3Trans[1].f = 1204.0f; float3Trans[2].f = 0.999f;
	pointLight.at(0).first.m_Translation = DirectX::XMFLOAT3(float3Trans[0].f, float3Trans[1].f, float3Trans[2].f);
	float3Colors[0].f = 0.75f; float3Colors[1].f = 1.0f; float3Colors[2].f = 0.999f;
	pointLight.at(0).first.m_Color = DirectX::XMFLOAT3(float3Colors[0].f, float3Colors[1].f, float3Colors[2].f);
	byteInt intTypeP;
	intTypeP.i = 1;
	pointLight.at(0).first.m_Type = intTypeP.i;
	floatIntensity.f = 1.f;
	pointLight.at(0).second.m_Intensity = floatIntensity.f;

	conv.setLevelPointLightList(&pointLight);

	std::vector<std::pair<LevelLoader::LightData, LevelLoader::SpotLight>> spotLight;
	spotLight.resize(1);
	float3Trans[0].f = 0.75f; float3Trans[1].f = 1204.0f; float3Trans[2].f = 0.999f;
	spotLight.at(0).first.m_Translation = DirectX::XMFLOAT3(float3Trans[0].f, float3Trans[1].f, float3Trans[2].f);
	float3Colors[0].f = 0.75f; float3Colors[1].f = 1.0f; float3Colors[2].f = 0.999f;
	spotLight.at(0).first.m_Color = DirectX::XMFLOAT3(float3Colors[0].f, float3Colors[1].f, float3Colors[2].f);
	byteInt intTypeS;
	intTypeS.i = 2;
	spotLight.at(0).first.m_Type = intTypeS.i;
	float3Direction[0].f = -0.159335f; float3Direction[1].f = -0.273098f; float3Direction[2].f = -0.948699f;  
	spotLight.at(0).second.m_Direction = DirectX::XMFLOAT3(float3Direction[0].f, float3Direction[1].f, float3Direction[2].f);
	floatIntensity.f = 1.f;
	spotLight.at(0).second.m_Intensity = floatIntensity.f;
	byteFloat floatConeAngle;
	floatConeAngle.f = 0.602f;
	spotLight.at(0).second.m_ConeAngle = floatConeAngle.f;
	byteFloat floatPenumbraAngle;
	floatPenumbraAngle.f = 0.102f;
	spotLight.at(0).second.m_PenumbraAngle = floatPenumbraAngle.f;

	conv.setLevelSpotLightList(&spotLight);

	std::ostringstream output;
	conv.testCreateLight(&output);

	std::string resLight = output.str();
	char size[] = "\x01\0\0\0";
	char diff[] = "\x03\0\0\0";
	float3Trans[0].f *= -1;
	float3Direction[0].f *= -1;
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin(), resLight.begin() + 4, diff, diff + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 4, resLight.begin() + 8, intTypeD.c , intTypeD.c + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 8, resLight.begin() + 12, size , size + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 12, resLight.begin() + 16, float3Trans[0].c , float3Trans[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 16, resLight.begin() + 20, float3Trans[1].c , float3Trans[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 20, resLight.begin() + 24, float3Trans[2].c , float3Trans[2].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 24, resLight.begin() + 28, float3Colors[0].c , float3Colors[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 28, resLight.begin() + 32, float3Colors[1].c , float3Colors[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 32, resLight.begin() + 36, float3Colors[2].c , float3Colors[2].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 36, resLight.begin() + 40, intTypeD.c , intTypeD.c + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 40, resLight.begin() + 44, floatIntensity.c , floatIntensity.c + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 44, resLight.begin() + 48, float3Direction[0].c , float3Direction[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 48, resLight.begin() + 52, float3Direction[1].c , float3Direction[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 52, resLight.begin() + 56, float3Direction[2].c , float3Direction[2].c + sizeof(float));

	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 56, resLight.begin() + 60, intTypeP.c , intTypeP.c + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 60, resLight.begin() + 64, size , size + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 64, resLight.begin() + 68, float3Trans[0].c , float3Trans[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 68, resLight.begin() + 72, float3Trans[1].c , float3Trans[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 72, resLight.begin() + 76, float3Trans[2].c , float3Trans[2].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 76, resLight.begin() + 80, float3Colors[0].c , float3Colors[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 80, resLight.begin() + 84, float3Colors[1].c , float3Colors[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 84, resLight.begin() + 88, float3Colors[2].c , float3Colors[2].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 88, resLight.begin() + 92, intTypeP.c , intTypeP.c + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 92, resLight.begin() + 96, floatIntensity.c , floatIntensity.c + sizeof(int));

	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 96, resLight.begin() + 100, intTypeS.c , intTypeS.c + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 100, resLight.begin() + 104, size , size + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 104, resLight.begin() + 108, float3Trans[0].c , float3Trans[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 108, resLight.begin() + 112, float3Trans[1].c , float3Trans[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 112, resLight.begin() + 116, float3Trans[2].c , float3Trans[2].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 116, resLight.begin() + 120, float3Colors[0].c , float3Colors[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 120, resLight.begin() + 124, float3Colors[1].c , float3Colors[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 124, resLight.begin() + 128, float3Colors[2].c , float3Colors[2].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 128, resLight.begin() + 132, intTypeS.c , intTypeS.c + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 132, resLight.begin() + 136, floatIntensity.c , floatIntensity.c + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 136, resLight.begin() + 140, float3Direction[0].c , float3Direction[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 140, resLight.begin() + 144, float3Direction[1].c , float3Direction[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 144, resLight.begin() + 148, float3Direction[2].c , float3Direction[2].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 148, resLight.begin() + 152, floatConeAngle.c , floatConeAngle.c + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resLight.begin() + 152, resLight.begin() + 156, floatPenumbraAngle.c , floatPenumbraAngle.c + sizeof(int));
}

BOOST_AUTO_TEST_CASE(TestCreateCheckPoints)
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
	struct byteInt
	{
		union
		{
			int i;
			char c[sizeof(int)];
		};
	};

	std::vector<LevelLoader::CheckPointStruct> checkPoints;
	checkPoints.resize(1);
	byteInt number;
	number.i = 3;
	checkPoints.at(0).m_Number = number.i;
	byteFloat float3Trans[3];
	float3Trans[0].f = 200.f; float3Trans[1].f = 20.f; float3Trans[2].f = 2.f;
	checkPoints.at(0).m_Translation = DirectX::XMFLOAT3(float3Trans[0].f, float3Trans[1].f, float3Trans[2].f);

	conv.setLevelCheckPointList(&checkPoints);

	DirectX::XMFLOAT3 checkPointsStart;
	byteFloat float3TransStart[3];
	float3TransStart[0].f = 200.f; float3TransStart[1].f = 20.f; float3TransStart[2].f = 2.f;
	checkPointsStart = DirectX::XMFLOAT3(float3TransStart[0].f, float3TransStart[1].f, float3TransStart[2].f);

	conv.setLevelCheckPointStart(checkPointsStart);

	DirectX::XMFLOAT3 checkPointsEnd;
	byteFloat float3TransEnd[3];
	float3TransEnd[0].f = 250.f; float3TransEnd[1].f = 20.f; float3TransEnd[2].f = 2.f;
	checkPointsEnd = DirectX::XMFLOAT3(float3TransEnd[0].f, float3TransEnd[1].f, float3TransEnd[2].f);

	conv.setLevelCheckPointEnd(checkPointsEnd);

	std::ostringstream output;
	conv.testCreateCheckPoints(&output);

	std::string resPoint = output.str();
	char size[] = "\x01\0\0\0";
	float3TransStart[0].f *= -1;
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin(), resPoint.begin() + 4, float3TransStart[0].c, float3TransStart[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin() + 4, resPoint.begin() + 8, float3TransStart[1].c, float3TransStart[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin() + 8, resPoint.begin() + 12, float3TransStart[2].c, float3TransStart[2].c + sizeof(float));
	float3TransEnd[0].f *= -1;
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin() + 12, resPoint.begin() + 16, float3TransEnd[0].c, float3TransEnd[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin() + 16, resPoint.begin() + 20, float3TransEnd[1].c, float3TransEnd[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin() + 20, resPoint.begin() + 24, float3TransEnd[2].c, float3TransEnd[2].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin() + 24, resPoint.begin() + 28, size , size + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin() + 28, resPoint.begin() + 32, number.c , number.c + sizeof(int));
	float3Trans[0].f *= -1;
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin() + 32, resPoint.begin() + 36, float3Trans[0].c, float3Trans[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin() + 36, resPoint.begin() + 40, float3Trans[1].c, float3Trans[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resPoint.begin() + 40, resPoint.begin() + 44, float3Trans[2].c, float3Trans[2].c + sizeof(float));
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
	
	static const char binHeader[] = 
		"\x01\0\0\0"
		"\x04\0\0\0Test"
		"\x01\0\0\0";

	std::vector<LevelLoader::ModelStruct> level;
	level.resize(1);
	level.at(0).m_MeshName = "Test";
	byteFloat float3Trans[3];
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
	float3Trans[0].f *= -1;
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 16, resHeader.begin() + 20, float3Trans[0].c , float3Trans[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 20, resHeader.begin() + 24, float3Trans[1].c , float3Trans[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 24, resHeader.begin() + 28, float3Trans[2].c , float3Trans[2].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 28, resHeader.begin() + 32, size , size + sizeof(int));
	float3Rot[0].f *= -1;
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 32, resHeader.begin() + 36, float3Rot[0].c , float3Rot[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 36, resHeader.begin() + 40, float3Rot[1].c , float3Rot[1].c + sizeof(float));
	float3Rot[2].f *= -1;
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 40, resHeader.begin() + 44, float3Rot[2].c , float3Rot[2].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 44, resHeader.begin() + 48, size , size + sizeof(int));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 48, resHeader.begin() + 52, float3Scal[0].c , float3Scal[0].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 52, resHeader.begin() + 56, float3Scal[1].c , float3Scal[1].c + sizeof(float));
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin() + 56, resHeader.begin() + 60, float3Scal[2].c , float3Scal[2].c + sizeof(float));
}

BOOST_AUTO_TEST_SUITE_END()