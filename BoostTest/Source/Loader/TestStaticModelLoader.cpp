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
	test = loader.loadFile("Source/Sample130.tx");
	BOOST_CHECK_EQUAL(test, true);

	std::vector<std::vector<ModelLoader::IndexDesc>> temp = loader.getIndices();

	BOOST_CHECK_EQUAL(temp.size(), loader.getNumberOfMaterial());

	std::vector<DirectX::XMFLOAT3> tempVer = loader.getVertices();

	BOOST_CHECK_EQUAL(tempVer.size(), loader.getNumberOfVertices());
}

BOOST_AUTO_TEST_SUITE_END()