#include <boost/test/unit_test.hpp>
#include "Animation.h"
#include "AnimationLoader.h"
#include "CommonExceptions.h"

/**
 * If these test break, go to dropbox and download the "TestCharacter.atx" file from the "Files needed for BoostTest" folder.
 */


BOOST_AUTO_TEST_SUITE(AnimationTest)

using namespace DirectX;

BOOST_AUTO_TEST_CASE(testAnimation)
{
	Animation testAnimation;
	AnimationLoader animationLoader;

	animationLoader.loadAnimationDataResource("testResource", "../Source/TestCharacter.atx");

	testAnimation.setAnimationData(animationLoader.getAnimationData("testResource"));

	BOOST_CHECK(testAnimation.getAnimationData() != nullptr);

	testAnimation.playClip("RunningJump", false);
	testAnimation.playClip("SideStepLeft", false);
	testAnimation.playClip("SideStepRight", false);
	testAnimation.playClip("CastSpell", false);

	testAnimation.queueClip("CastSpell");
	testAnimation.queueClip("CastSpell");
	testAnimation.changeWeight(2, 0.5f);
	testAnimation.updateAnimation(0.015f);

	testAnimation.updateAnimation(0.6f);
	testAnimation.updateAnimation(.2f);

	DirectX::XMMATRIX w = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT4X4 fw;
	DirectX::XMStoreFloat4x4(&fw, w);
	DirectX::XMFLOAT3 reachPoint = testAnimation.getJointPos("Neck", fw);

	testAnimation.applyIK_ReachPoint("LeftArm", reachPoint, fw);
	DirectX::XMFLOAT3 reachedPoint = testAnimation.getJointPos("L_Hand", fw);
	BOOST_CHECK(abs(abs(reachedPoint.x) - abs(reachPoint.x)) < 1.0f);
	BOOST_CHECK(abs(abs(reachedPoint.y) - abs(reachPoint.y)) < 1.0f);
	BOOST_CHECK(abs(abs(reachedPoint.z) - abs(reachPoint.z)) < 1.0f);

	BOOST_CHECK_THROW(testAnimation.getJointPos("ASDF", fw), InvalidArgument);

	testAnimation.getFinalTransform();
}

BOOST_AUTO_TEST_SUITE_END()