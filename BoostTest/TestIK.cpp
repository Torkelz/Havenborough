//#include <boost/test/unit_test.hpp>
//#include "../../Graphics/Source/ModelInstance.h"
//
//namespace DirectX
//{
//	std::ostream& operator<< (std::ostream& os, const XMFLOAT3& vec)
//	{
//		return os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
//	}
//
//	bool operator==(const XMFLOAT3& left, const XMFLOAT3& right)
//	{
//		return left.x == right.x && left.y == right.y && left.z == right.z;
//	}
//}
//
//#define CHECK_CLOSE(L, R, T) \
//	if (std::fabs(R) < T) \
//	{ \
//		BOOST_CHECK_SMALL(L, T); \
//	} \
//	else \
//	{ \
//		BOOST_CHECK_CLOSE_FRACTION(L, R, T); \
//	}
//
//#define CHECK_CLOSE_VEC3(L, R, T) \
//	do { \
//	XMFLOAT3 left = L; \
//	XMFLOAT3 right = R; \
//	CHECK_CLOSE(left.x, right.x, T) \
//	CHECK_CLOSE(left.y, right.y, T) \
//	CHECK_CLOSE(left.z, right.z, T) \
//	} while (false)
//
//BOOST_AUTO_TEST_SUITE(TestIK)
//
//	BOOST_AUTO_TEST_CASE(TestStaticReachPoint)
//	{
//		using namespace DirectX;
//
//		static const std::string shoulderJointName = "TestJoint1";
//		static const std::string elbowJointName = "TestJoint2";
//		static const std::string handJointName = "TestJoint3";
//
//		std::vector<Joint> joints;
//		Joint joint;
//		joint.m_Parent = 0;
//		joint.m_ID = 1;
//		KeyFrame keyFrame;
//		keyFrame.m_Rot = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
//		keyFrame.m_Scale = XMFLOAT3(1.f, 1.f, 1.f);
//		keyFrame.m_Trans = XMFLOAT3(0.f, 0.f, 0.f);
//		joint.m_JointAnimation.push_back(keyFrame);
//		joint.m_JointAnimation.push_back(keyFrame);
//		joint.m_JointName = shoulderJointName;
//		joint.m_JointOffsetMatrix = XMFLOAT4X4(1.f, 0.f, 0.f, 0.f,
//												0.f, 1.f, 0.f, 0.f,
//												0.f, 0.f, 1.f, 0.f,
//												0.f, 0.f, 0.f, 1.f);
//		joint.m_TotalJointOffset = joint.m_JointOffsetMatrix;
//		joints.push_back(joint);
//
//		joint.m_Parent = 1;
//		joint.m_ID = 2;
//		joint.m_JointAnimation[0].m_Trans.x = 4.f;
//		joint.m_JointAnimation[1].m_Trans.x = 4.f;
//		joint.m_JointName = elbowJointName;
//		joint.m_JointOffsetMatrix._41 = 4.f;
//		joint.m_TotalJointOffset._14 = 4.f;
//		joints.push_back(joint);
//
//		joint.m_Parent = 2;
//		joint.m_ID = 3;
//		joint.m_JointName = handJointName;
//		joint.m_TotalJointOffset._14 = 8.f;
//		joints.push_back(joint);
//
//		ModelInstance model;
//		model.setModelName("TestModel");
//		model.setPosition(XMFLOAT3(0.f, 0.f, 0.f));
//		model.setRotation(XMFLOAT3(0.f, 0.f, 0.f));
//		model.setScale(XMFLOAT3(1.f, 1.f, 1.f));
//
//		model.updateAnimation(0.f, joints);
//		
//		BOOST_CHECK_EQUAL(model.getJointPos(shoulderJointName, joints), XMFLOAT3(0.f, 0.f, 0.f));
//		BOOST_CHECK_EQUAL(model.getJointPos(elbowJointName, joints), XMFLOAT3(-4.f, 0.f, 0.f));
//		BOOST_CHECK_EQUAL(model.getJointPos(handJointName, joints), XMFLOAT3(-8.f, 0.f, 0.f));
//
//		model.applyIK_ReachPoint(handJointName, elbowJointName, shoulderJointName, XMFLOAT3(-4.f, 4.f, 0.f), joints);
//
//		CHECK_CLOSE_VEC3(model.getJointPos(shoulderJointName, joints), XMFLOAT3(0.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(elbowJointName, joints), XMFLOAT3(-4.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(handJointName, joints), XMFLOAT3(-4.f, 4.f, 0.f), 0.01f);
//
//		model.applyIK_ReachPoint(handJointName, elbowJointName, shoulderJointName, XMFLOAT3(-4.f, 4.f, 0.f), joints);
//
//		CHECK_CLOSE_VEC3(model.getJointPos(shoulderJointName, joints), XMFLOAT3(0.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(elbowJointName, joints), XMFLOAT3(-4.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(handJointName, joints), XMFLOAT3(-4.f, 4.f, 0.f), 0.01f);
//
//		model.updateAnimation(0.f, joints);
//		model.applyIK_ReachPoint(handJointName, elbowJointName, shoulderJointName, XMFLOAT3(-8.f, 0.f, 0.f), joints);
//
//		CHECK_CLOSE_VEC3(model.getJointPos(shoulderJointName, joints), XMFLOAT3(0.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(elbowJointName, joints), XMFLOAT3(-4.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(handJointName, joints), XMFLOAT3(-8.f, 0.f, 0.f), 0.01f);
//
//		model.updateAnimation(0.f, joints);
//		model.applyIK_ReachPoint(handJointName, elbowJointName, shoulderJointName, XMFLOAT3(8.f, 0.0001f, 0.f), joints);
//
//		CHECK_CLOSE_VEC3(model.getJointPos(shoulderJointName, joints), XMFLOAT3(0.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(elbowJointName, joints), XMFLOAT3(4.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(handJointName, joints), XMFLOAT3(8.f, 0.f, 0.f), 0.01f);
//
//		static const XMFLOAT3 extraTests[] =
//		{
//			XMFLOAT3(0.f, 8.f, 0.f),
//			XMFLOAT3(0.f, -8.f, 0.f),
//			XMFLOAT3(0.f, 4.f, 4.f),
//			XMFLOAT3(2.f, 2.f, 2.f),
//			XMFLOAT3(-1.f, 5.f, 3.f),
//		};
//
//		for (const auto& test : extraTests)
//		{
//			model.updateAnimation(0.f, joints);
//			model.applyIK_ReachPoint(handJointName, elbowJointName, shoulderJointName, test, joints);
//
//			CHECK_CLOSE_VEC3(model.getJointPos(handJointName, joints), test, 0.01f);
//		}
//	}
//
//	BOOST_AUTO_TEST_CASE(TestAnimatedReachPoint)
//	{
//		using namespace DirectX;
//
//		static const std::string shoulderJointName = "TestJoint1";
//		static const std::string elbowJointName = "TestJoint2";
//		static const std::string handJointName = "TestJoint3";
//
//		std::vector<Joint> joints;
//		Joint joint;
//		joint.m_Parent = 0;
//		joint.m_ID = 1;
//		KeyFrame keyFrame;
//		XMStoreFloat4(&keyFrame.m_Rot, XMQuaternionRotationRollPitchYaw(1.f, 3.f, 2.f));
//		keyFrame.m_Scale = XMFLOAT3(1.f, 1.f, 1.f);
//		keyFrame.m_Trans = XMFLOAT3(0.f, 0.f, 0.f);
//		joint.m_JointAnimation.push_back(keyFrame);
//		joint.m_JointAnimation.push_back(keyFrame);
//		joint.m_JointName = shoulderJointName;
//		joint.m_JointOffsetMatrix = XMFLOAT4X4(1.f, 0.f, 0.f, 0.f,
//												0.f, 1.f, 0.f, 0.f,
//												0.f, 0.f, 1.f, 0.f,
//												0.f, 0.f, 0.f, 1.f);
//		joint.m_TotalJointOffset = joint.m_JointOffsetMatrix;
//		joints.push_back(joint);
//
//		joint.m_Parent = 1;
//		joint.m_ID = 2;
//		joint.m_JointAnimation[0].m_Trans.x = 4.f;
//		XMStoreFloat4(&joint.m_JointAnimation[0].m_Rot, XMQuaternionRotationRollPitchYaw(1.f, 0.f, 0.f));
//		joint.m_JointAnimation[1].m_Trans.x = 4.f;
//		XMStoreFloat4(&joint.m_JointAnimation[1].m_Rot, XMQuaternionRotationRollPitchYaw(1.f, 0.f, 0.f));
//		joint.m_JointName = elbowJointName;
//		joint.m_JointOffsetMatrix._41 = 4.f;
//		joint.m_TotalJointOffset._14 = 4.f;
//		joints.push_back(joint);
//
//		joint.m_Parent = 2;
//		joint.m_ID = 3;
//		XMStoreFloat4(&joint.m_JointAnimation[0].m_Rot, XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f));
//		XMStoreFloat4(&joint.m_JointAnimation[1].m_Rot, XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f));
//		joint.m_JointName = handJointName;
//		joint.m_TotalJointOffset._14 = 8.f;
//		joints.push_back(joint);
//
//		ModelInstance model;
//		model.setModelName("TestModel");
//		model.setPosition(XMFLOAT3(0.f, 0.f, 0.f));
//		model.setRotation(XMFLOAT3(0.f, 0.f, 0.f));
//		model.setScale(XMFLOAT3(1.f, 1.f, 1.f));
//
//		model.updateAnimation(0.f, joints);
//		
//		CHECK_CLOSE_VEC3(model.getJointPos(shoulderJointName, joints), XMFLOAT3(0.f, 0.f, 0.f), 0.01f);
//
//		model.applyIK_ReachPoint(handJointName, elbowJointName, shoulderJointName, XMFLOAT3(-4.f, 4.f, 0.f), joints);
//
//		CHECK_CLOSE_VEC3(model.getJointPos(shoulderJointName, joints), XMFLOAT3(0.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(handJointName, joints), XMFLOAT3(-4.f, 4.f, 0.f), 0.01f);
//
//		model.applyIK_ReachPoint(handJointName, elbowJointName, shoulderJointName, XMFLOAT3(-4.f, 4.f, 0.f), joints);
//
//		CHECK_CLOSE_VEC3(model.getJointPos(shoulderJointName, joints), XMFLOAT3(0.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(handJointName, joints), XMFLOAT3(-4.f, 4.f, 0.f), 0.01f);
//
//		model.updateAnimation(0.f, joints);
//		model.applyIK_ReachPoint(handJointName, elbowJointName, shoulderJointName, XMFLOAT3(-8.f, 0.f, 0.f), joints);
//
//		CHECK_CLOSE_VEC3(model.getJointPos(shoulderJointName, joints), XMFLOAT3(0.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(handJointName, joints), XMFLOAT3(-8.f, 0.f, 0.f), 0.01f);
//
//		model.updateAnimation(0.f, joints);
//		model.applyIK_ReachPoint(handJointName, elbowJointName, shoulderJointName, XMFLOAT3(8.f, 0.0001f, 0.f), joints);
//
//		CHECK_CLOSE_VEC3(model.getJointPos(shoulderJointName, joints), XMFLOAT3(0.f, 0.f, 0.f), 0.01f);
//		CHECK_CLOSE_VEC3(model.getJointPos(handJointName, joints), XMFLOAT3(8.f, 0.f, 0.f), 0.01f);
//
//		static const XMFLOAT3 extraTests[] =
//		{
//			XMFLOAT3(0.f, 8.f, 0.f),
//			XMFLOAT3(0.f, -8.f, 0.f),
//			XMFLOAT3(0.f, 4.f, 4.f),
//			XMFLOAT3(2.f, 2.f, 2.f),
//			XMFLOAT3(-1.f, 5.f, 3.f),
//		};
//
//		for (const auto& test : extraTests)
//		{
//			model.updateAnimation(0.f, joints);
//			model.applyIK_ReachPoint(handJointName, elbowJointName, shoulderJointName, test, joints);
//
//			CHECK_CLOSE_VEC3(model.getJointPos(handJointName, joints), test, 0.01f);
//		}
//	}
//
//BOOST_AUTO_TEST_SUITE_END()
