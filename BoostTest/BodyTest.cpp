#include <boost\test\unit_test.hpp>
#include "..\..\Physics\Source\Body.h"

BOOST_AUTO_TEST_SUITE(BodyTesting)

class stubBody : public Body
{
public:
	
	stubBody()
	{
		m_Mass				= 1.f;	
		m_Position			= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		m_NetForce			= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Velocity			= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Acceleration		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_LastAcceleration	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_AvgAcceleration	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_NewAcceleration	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Gravity			= 0.f;

		
		m_InAir				= true;
		m_IsImmovable		= false;

		DirectX::XMStoreFloat4x4(&m_Orientation, DirectX::XMMatrixIdentity());
	}



	//DirectX::XMFLOAT4 calculateAcceleration()
	//{
	//	return DirectX::XMFLOAT4(1.f,1.f,1.f,1.f);
	//}

private:

	void updateBoundingVolumePosition(DirectX::XMFLOAT4 p_Position) override
	{
		return;
	}

};

BOOST_AUTO_TEST_CASE(BodyTest_AddForce)
{
	stubBody fakeBody = stubBody();
	
	DirectX::XMFLOAT4 m_Netforce = fakeBody.getNetForce();
	BOOST_CHECK_EQUAL(m_Netforce.x, 0.f); 
	BOOST_CHECK_EQUAL(m_Netforce.y, 0.f); 
	BOOST_CHECK_EQUAL(m_Netforce.z, 0.f);

	fakeBody.addForce(DirectX::XMFLOAT4(1.f, 0.f, 3.f, 0.f));
	fakeBody.addForce(DirectX::XMFLOAT4(1.f, 0.f, 3.f, 0.f));

	m_Netforce = fakeBody.getNetForce();
	BOOST_CHECK_EQUAL(m_Netforce.x, 2.f);
	BOOST_CHECK_EQUAL(m_Netforce.y, 0.f); 
	BOOST_CHECK_EQUAL(m_Netforce.z, 6.f);

}

BOOST_AUTO_TEST_CASE(BodyTest_GetandSetInAir)
{
	stubBody fakeBody = stubBody();

	BOOST_CHECK_EQUAL(fakeBody.getInAir(), true);

	fakeBody.setInAir(false);

	BOOST_CHECK_EQUAL(fakeBody.getInAir(), false);
}

BOOST_AUTO_TEST_CASE(BodyTest_GetandSetGravity)
{
	stubBody fakeBody = stubBody();

	BOOST_CHECK_EQUAL(fakeBody.getGravity(), 0.f);

	fakeBody.setGravity(13.37f);

	BOOST_CHECK_EQUAL(fakeBody.getGravity(), 13.37f);
}
BOOST_AUTO_TEST_CASE(BodyTest_SetPosition)
{
	stubBody fakeBody = stubBody();

	BOOST_CHECK_EQUAL(fakeBody.getPosition().x, 0.f);
	BOOST_CHECK_EQUAL(fakeBody.getPosition().y, 0.f);
	BOOST_CHECK_EQUAL(fakeBody.getPosition().z, 0.f);
	BOOST_CHECK_EQUAL(fakeBody.getPosition().w, 1.f);

	fakeBody.setPosition(DirectX::XMFLOAT4(5.f, 10.f, 15.f, 1.f));
	fakeBody.setPosition(DirectX::XMFLOAT4(5.f, 10.f, 15.f, 1.f));

	BOOST_CHECK_EQUAL(fakeBody.getPosition().x, 5.f);
	BOOST_CHECK_EQUAL(fakeBody.getPosition().y, 10.f);
	BOOST_CHECK_EQUAL(fakeBody.getPosition().z, 15.f);
	BOOST_CHECK_EQUAL(fakeBody.getPosition().w, 1.f);
}


BOOST_AUTO_TEST_CASE(BodyTest_Update)
{
	stubBody fakeBody = stubBody();

	DirectX::XMFLOAT4 m_Acceleration = fakeBody.getACC();
	BOOST_CHECK_EQUAL(m_Acceleration.x, 0.f);
	BOOST_CHECK_EQUAL(m_Acceleration.y, 0.f);
	BOOST_CHECK_EQUAL(m_Acceleration.z, 0.f);

	fakeBody.addForce(DirectX::XMFLOAT4(5.f, 0.f, 0.f, 0.f));

	float timestep = 1.f / 60.f;

	fakeBody.update(timestep);

	m_Acceleration = fakeBody.getACC();
	BOOST_CHECK_EQUAL(m_Acceleration.x, 5.f);
	BOOST_CHECK_EQUAL(m_Acceleration.y, 0.f);
	BOOST_CHECK_EQUAL(m_Acceleration.z, 0.f);

	DirectX::XMFLOAT4 m_LastAcceleration = fakeBody.getLastACC();
	BOOST_CHECK_EQUAL(m_LastAcceleration.x, 0.f);
	BOOST_CHECK_EQUAL(m_LastAcceleration.y, 0.f);
	BOOST_CHECK_EQUAL(m_LastAcceleration.z, 0.f);

	fakeBody.update(timestep);

	m_LastAcceleration = fakeBody.getLastACC();
	BOOST_CHECK_EQUAL(m_LastAcceleration.x, 2.5f);
	BOOST_CHECK_EQUAL(m_LastAcceleration.y, 0.f);
	BOOST_CHECK_EQUAL(m_LastAcceleration.z, 0.f);


}

BOOST_AUTO_TEST_SUITE_END()