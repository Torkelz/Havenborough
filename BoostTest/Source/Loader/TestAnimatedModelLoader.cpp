#include <boost/test/unit_test.hpp>
#include "../../../Graphics/Source/ModelLoader.h"

BOOST_AUTO_TEST_SUITE(TestAnimatedModelLoader)


BOOST_AUTO_TEST_CASE(TestMissingFile)
{
	ModelLoader loader;
	bool test;
	test = loader.loadFile("Sample140.tx");
	BOOST_CHECK_EQUAL(test, false);
}

BOOST_AUTO_TEST_CASE(TestLoadingFile)
{
	ModelLoader loader;
	bool test;
	test = loader.loadFile("Source/Sample150.tx");
	BOOST_CHECK_EQUAL(test, true);

	const std::vector<std::vector<ModelLoader::IndexDesc>>& temp = loader.getIndices();

	BOOST_CHECK_EQUAL(temp.size(), loader.getNumberOfMaterial());

	const std::vector<DirectX::XMFLOAT3>& tempVer = loader.getVertices();

	BOOST_CHECK_EQUAL(tempVer.size(), loader.getNumberOfVertices());
	
	loader.loadFile("Source/Sample150.tx");
	const std::vector<std::vector<ModelLoader::IndexDesc>>& temp2 = loader.getIndices();

	BOOST_CHECK_EQUAL(temp2.size(), loader.getNumberOfMaterial());

	const std::vector<Joint>& tempJoints = loader.getListOfJoints();

	BOOST_CHECK_EQUAL(tempJoints.at(0).m_JointAnimation.size(), loader.getNumberOfFrames());
}

BOOST_AUTO_TEST_SUITE_END()