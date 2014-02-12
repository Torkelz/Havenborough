#include <boost/test/unit_test.hpp>
#include "../../../BinaryConverter/Source/InstanceLoader.h"

BOOST_AUTO_TEST_SUITE(TestLevelLoader)

class testLevelLoader : public InstanceLoader
{
public:
	int testHeader(std::istream& p_Input)
	{
		return readHeader(p_Input);
	}
	void testMeshList(std::istream& p_Input)
	{
		readMeshList(p_Input);
	}
	void testLightLists(std::istream& p_Input)
	{
		readLightList(p_Input);
	}
	void testCheckPointList(std::istream& p_Input)
	{
		readCheckPointList(p_Input);
	}
	void testMainLoop(std::istream& p_Input)
	{
		startReading(p_Input);
	}
	void testModelHeader(std::string p_FilePath)
	{
		readModelHeaders(p_FilePath);
	}
};

BOOST_AUTO_TEST_CASE(TestReadHeader)
{
	std::string string = "#num: 512";
	std::istringstream stream(string); 
	testLevelLoader loader;
	
	int result = loader.testHeader(stream);

	BOOST_CHECK_EQUAL(result, 512);
	loader.clear();
}

BOOST_AUTO_TEST_CASE(TestReadMeshList)
{
	std::string string = "#Translation: 0 0 0 \n#Rotation: 0 1 0 \n#Scale: 1.138714 1.138714 1.138714";
	std::istringstream stream(string); 
	testLevelLoader loader;
	
	loader.testMeshList(stream);

	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Scale.x, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Scale.y, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Scale.z, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Rotation.x, 0);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Rotation.y, 1);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Rotation.z, 0);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Translation.x, 0);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Translation.y, 0);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Translation.z, 0);
	loader.clear();
}

BOOST_AUTO_TEST_CASE(TestReadLightList)
{
	std::string string =	"#Translation: -64.818832 86.49115 0\nColor: 1 1 1\nType: kDirectionalLight\nIntensity: 1\nDirection: -0.159335 -0.273098 -0.948699\n"
							"#Translation: 4942.175293 833.546997 -3392.910889\nColor: 1 1 1\nType: kPointLight\nIntensity: 0.2\n"
							"#Translation: -78.205856 2232.998291 0\nColor: 1 1 1\nType: kSpotLight\nIntensity: 1\nDirection: 0 -1425.282715 80.049324\nConeAngle: 1.062977 0\n";
	std::istringstream stream(string); 
	testLevelLoader loader;
	for(int i = 0; i < 3; i++)
	{
		loader.testLightLists(stream);
	}
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Translation.x, -64.818832f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Translation.y, 86.49115f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Translation.z, 0.0f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Color.x, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Color.y, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Color.z, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Type, 0);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).second.m_Intensity, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).second.m_Direction.x, -0.159335f); 
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).second.m_Direction.y, -0.273098f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).second.m_Direction.z, -0.948699f);

	BOOST_CHECK_EQUAL(loader.getLevelPointLightList().at(0).first.m_Translation.x, 4942.175293f);
	BOOST_CHECK_EQUAL(loader.getLevelPointLightList().at(0).first.m_Translation.y, 833.546997f);
	BOOST_CHECK_EQUAL(loader.getLevelPointLightList().at(0).first.m_Translation.z, -3392.910889f);
	BOOST_CHECK_EQUAL(loader.getLevelPointLightList().at(0).first.m_Color.x, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelPointLightList().at(0).first.m_Color.y, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelPointLightList().at(0).first.m_Color.z, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelPointLightList().at(0).first.m_Type, 1);
	BOOST_CHECK_EQUAL(loader.getLevelPointLightList().at(0).second.m_Intensity, 0.2f);

	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).first.m_Translation.x, -78.205856f); 
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).first.m_Translation.y, 2232.998291f);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).first.m_Translation.z, 0.0f);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).first.m_Color.x, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).first.m_Color.y, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).first.m_Color.z, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).first.m_Type, 2);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).second.m_Intensity, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).second.m_Direction.x, 0.0f);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).second.m_Direction.y, -1425.282715f);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).second.m_Direction.z, 80.049324f);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).second.m_ConeAngle, 1.062977f);
	BOOST_CHECK_EQUAL(loader.getLevelSpotLightList().at(0).second.m_PenumbraAngle, 0.0f);
	loader.clear();
}

BOOST_AUTO_TEST_CASE(TestCheckPointList)
{
	std::string string = "#NR: 3\nTranslation: 53.728424 42.451406 159.208494\n";
	std::istringstream stream(string); 
	testLevelLoader loader;
	
	loader.testCheckPointList(stream);

	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(0).m_Number, 3);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(0).m_Translation.x, 53.728424f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(0).m_Translation.y, 42.451406f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(0).m_Translation.z, 159.208494f);
	loader.clear();
}

BOOST_AUTO_TEST_CASE(TestMainLevelLoop)
{
	std::string string = "*ObjectHeader* \n#numObj: 512 \n\n#MESH: House1 \n#Translation: 0 0 0 \n#Rotation: 0 1 0 \n#Scale: 1.138714 1.138714 1.138714\n\n"
		"*LightHeader*\n#numLights 12\n\n#Light: directionalLightShape1\n#Translation: -64.818832 86.49115 0\nColor: 1 1 1\nType: kDirectionalLight\nIntensity: 1\nDirection: -0.159335 -0.273098 -0.948699\n\n"
		"*CheckPointHeader*\n#numCheckpoints 3\n\n#Type: CheckPoint\n#NR: 1\nTranslation: 0 42.451406 106.708522\n\n#Type: CheckPoint\n#NR: 2\nTranslation: 0 42.451406 106.708522\n\n#Type: CheckPoint\n#NR: 3\nTranslation: 0 42.451406 106.708522\n\n"
		"#Type: Start\nTranslation: 53.728424 42.451406 62.230928\n\n#Type: End\nTranslation: 53.728424 42.451406 102.913386\n";
	std::istringstream stream(string); 
	testLevelLoader loader;
	
	loader.testMainLoop(stream);

	BOOST_CHECK_EQUAL(loader.getLevelHeader().m_NumberOfModels, 512);
	BOOST_CHECK_EQUAL(loader.getLevelHeader().m_NumberOfLights, 12);
	BOOST_CHECK_EQUAL(loader.getLevelHeader().m_NumberOfCheckPoints, 3);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_MeshName, "House1");
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Scale.x, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Scale.y, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Scale.z, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Rotation.x, 0);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Rotation.y, 1);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Rotation.z, 0);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Translation.x, 0);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Translation.y, 0);
	BOOST_CHECK_EQUAL(loader.getModelList().at(0).m_Translation.z, 0);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Translation.x, -64.818832f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Translation.y, 86.49115f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Translation.z, 0.0f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Color.x, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Color.y, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Color.z, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).first.m_Type, 0);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).second.m_Intensity, 1.0f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).second.m_Direction.x, -0.159335f); 
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).second.m_Direction.y, -0.273098f);
	BOOST_CHECK_EQUAL(loader.getLevelDirectionalLightList().at(0).second.m_Direction.z, -0.948699f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(0).m_Number, 1);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(0).m_Translation.x, 0.f); 
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(0).m_Translation.y, 42.451406f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(0).m_Translation.z, 106.708522f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(1).m_Number, 2);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(1).m_Translation.x, 0.f); 
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(1).m_Translation.y, 42.451406f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(1).m_Translation.z, 106.708522f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(2).m_Number, 3);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(2).m_Translation.x, 0.f); 
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(2).m_Translation.y, 42.451406f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointList().at(2).m_Translation.z, 106.708522f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointStart().x, 53.728424f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointStart().y, 42.451406f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointStart().z, 62.230928f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointEnd().x, 53.728424f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointEnd().y, 42.451406f);
	BOOST_CHECK_EQUAL(loader.getLevelCheckPointEnd().z, 102.913386f);
	loader.clear();
}

BOOST_AUTO_TEST_CASE(TestModelHeaderReader)
{
	testLevelLoader loader;
	loader.testModelHeader("..\\Source\\Loader\\testModelHeader.txx");

	BOOST_CHECK_EQUAL(loader.getModelInformation().at(0).m_Animated, 0);
	BOOST_CHECK_EQUAL(loader.getModelInformation().at(0).m_Collidable, 0);
	BOOST_CHECK_EQUAL(loader.getModelInformation().at(0).m_Transparency, 0);
	loader.clear();
}

BOOST_AUTO_TEST_SUITE_END()