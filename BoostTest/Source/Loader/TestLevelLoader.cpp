#include <boost/test/unit_test.hpp>
#include "../../../BinaryConverter/Source/LevelLoader.h"

BOOST_AUTO_TEST_SUITE(TestLevelLoader)

class testLevelLoader : public LevelLoader
{
public:
	void testHeader(std::istream& p_Input)
	{
		readHeader(p_Input);
	}
	void testMeshList(std::istream& p_Input)
	{
		readMeshList(p_Input);
	}
	void testMainLoop(std::istream& p_Input)
	{
		startReading(p_Input);
	}
};

BOOST_AUTO_TEST_CASE(TestReadHeader)
{
	std::string string = "#numObj: 512";
	std::istringstream stream(string); 
	testLevelLoader loader;
	
	loader.testHeader(stream);

	BOOST_CHECK_EQUAL(loader.getLevelHeader().m_NumberOfModels, 512);
}

BOOST_AUTO_TEST_CASE(TestReadMeshList)
{
	std::string string = "#Translation: 0 0 0 \n#Rotation: 0 1 0 \n#Scale: 1.138714 1.138714 1.138714";
	std::istringstream stream(string); 
	testLevelLoader loader;
	
	loader.testMeshList(stream);

	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Scale.x, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Scale.y, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Scale.z, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Rotation.x, 0);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Rotation.y, 1);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Rotation.z, 0);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Translation.x, 0);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Translation.y, 0);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Translation.z, 0);
}

BOOST_AUTO_TEST_CASE(TestMainLoop)
{
	std::string string = "*ObjectHeader* \n#numObj: 512 \n\n#MESH: House1 \n#Translation: 0 0 0 \n#Rotation: 0 1 0 \n#Scale: 1.138714 1.138714 1.138714";
	std::istringstream stream(string); 
	testLevelLoader loader;
	
	loader.testMainLoop(stream);

	BOOST_CHECK_EQUAL(loader.getLevelHeader().m_NumberOfModels, 512);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_MeshName, "House1");
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Scale.x, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Scale.y, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Scale.z, 1.13871396f);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Rotation.x, 0);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Rotation.y, 1);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Rotation.z, 0);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Translation.x, 0);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Translation.y, 0);
	BOOST_CHECK_EQUAL(loader.getLevelModelList().at(0).m_Translation.z, 0);
}

BOOST_AUTO_TEST_SUITE_END()