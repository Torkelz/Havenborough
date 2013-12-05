#include <boost/test/unit_test.hpp>
#include "../../../Graphics/Source/ModelLoader.h"

BOOST_AUTO_TEST_SUITE(TestStaticModelLoader)


BOOST_AUTO_TEST_CASE(TestMissingFile)
{
	ModelLoader loader;
	bool test;
	test = loader.loadFile("Sample130.tx");
	BOOST_CHECK_EQUAL(test, false);
}

BOOST_AUTO_TEST_CASE(TestLoadingFile)
{
	ModelLoader loader;
	bool test;
	test = loader.loadFile("Source/Sample140.tx");
	BOOST_CHECK_EQUAL(test, true);

	std::vector<std::vector<ModelLoader::IndexDesc>> temp = loader.getIndices();

	//BOOST_CHECK_EQUAL(temp.size(), 2);

	//BOOST_CHECK_EQUAL(temp.at(0).at(0).m_MaterialID, "-blinn1");

	std::vector<DirectX::XMFLOAT3> tempVer = loader.getVertices();

	//BOOST_CHECK_EQUAL(tempVer.size(), 8);

	std::vector<DirectX::XMFLOAT3> tempNor = loader.getNormals();

	//BOOST_CHECK_EQUAL(tempNor.size(), 24);

	std::vector<DirectX::XMFLOAT3> tempTan = loader.getTangents();

	//BOOST_CHECK_EQUAL(tempTan.size(), 36);

	std::vector<DirectX::XMFLOAT2> tempUV = loader.getTextureCoords();

	//BOOST_CHECK_EQUAL(tempUV.size(), 14);
}

BOOST_AUTO_TEST_SUITE_END()