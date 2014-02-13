#include <boost/test/unit_test.hpp>
#include "AnimationClipLoader.h"

BOOST_AUTO_TEST_SUITE(MLXLoaderTest)

using namespace DirectX;

BOOST_AUTO_TEST_CASE(checkAnimationClipCorrectLoad)
{
	std::map<std::string, AnimationClip> testClips = MattiasLucaseXtremeLoader::loadAnimationClip("../Source/Test.mlx");
	BOOST_CHECK(testClips.count("BindPose") > 0);
}

BOOST_AUTO_TEST_CASE(checkAnimationClipIncorrectLoad)
{
	std::map<std::string, AnimationClip> testClips = MattiasLucaseXtremeLoader::loadAnimationClip("../Source/Test3.mlx");
	BOOST_CHECK(testClips.count("BindPose") == 0);
}

BOOST_AUTO_TEST_CASE(checkIKGroupCorrectLoad)
{
	std::map<std::string, IKGroup> testClips = MattiasLucaseXtremeLoader::loadIKGroup("../Source/Test.mlx");
	BOOST_CHECK(testClips.count("LeftArm") > 0);
}

BOOST_AUTO_TEST_CASE(checkIKGroupIncorrectLoad)
{
	std::map<std::string, IKGroup> testClips = MattiasLucaseXtremeLoader::loadIKGroup("../Source/Test3.mlx");
	BOOST_CHECK(testClips.count("LeftArm") == 0);
}

BOOST_AUTO_TEST_CASE(checkAnimationPathCorrectLoad)
{
	std::map<std::string, AnimationPath> testClips = MattiasLucaseXtremeLoader::loadAnimationPath("../Source/Test.mlx");
	BOOST_CHECK(testClips.count("Climb1") > 0);
}

BOOST_AUTO_TEST_CASE(checkAnimationPathIncorrectLoad)
{
	std::map<std::string, AnimationPath> testClips = MattiasLucaseXtremeLoader::loadAnimationPath("../Source/Test3.mlx");
	BOOST_CHECK(testClips.count("Climb1") == 0);
}

BOOST_AUTO_TEST_SUITE_END()