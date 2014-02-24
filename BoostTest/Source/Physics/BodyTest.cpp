#include <boost\test\unit_test.hpp>
#include "..\..\Physics\Source\Body.h"
#include "..\..\Physics\Source\PhysicsExceptions.h"
#include "..\..\Physics\include\BoundingVolume.h"

class DummyBoundingVolume : public BoundingVolume
{
public:
	DummyBoundingVolume()
	{
		m_Type = Type::NONE;
		m_BodyHandle = 0;
	}
	~DummyBoundingVolume(){}

	void updatePosition(DirectX::XMFLOAT4X4 const &p_Translation) override
	{
		
	}
	
	void setPosition(DirectX::XMVECTOR const &p_Position) override
	{
		DirectX::XMStoreFloat4(&m_Position, p_Position);
	}

	void scale(DirectX::XMVECTOR const &p_Scale) override
	{
		
	}

	void setRotation(DirectX::XMMATRIX const &p_Rotation) override
	{
		
	}
};

BOOST_AUTO_TEST_SUITE(BodyTesting)


BOOST_AUTO_TEST_CASE(BodyTest_AddForce)
{
	Body body = Body(1.f, nullptr, false, false);
	
	DirectX::XMFLOAT4 m_Netforce = body.getNetForce();
	BOOST_CHECK_EQUAL(m_Netforce.x, 0.f); 
	BOOST_CHECK_EQUAL(m_Netforce.y, 0.f); 
	BOOST_CHECK_EQUAL(m_Netforce.z, 0.f);

	body.addForce(DirectX::XMFLOAT4(1.f, 0.f, 3.f, 0.f));
	body.addForce(DirectX::XMFLOAT4(1.f, 0.f, 3.f, 0.f));

	m_Netforce = body.getNetForce();
	BOOST_CHECK_EQUAL(m_Netforce.x, 2.f);
	BOOST_CHECK_EQUAL(m_Netforce.y, 0.f); 
	BOOST_CHECK_EQUAL(m_Netforce.z, 6.f);

}

BOOST_AUTO_TEST_CASE(BodyTest_GetandSetInAir)
{
	Body body = Body(1.f, nullptr, false, false);

	BOOST_CHECK_EQUAL(body.getInAir(), true);

	body.setInAir(false);

	BOOST_CHECK_EQUAL(body.getInAir(), false);
}

BOOST_AUTO_TEST_CASE(BodyTest_GetandSetGravity)
{
	Body body = Body(1.f, nullptr, false, false);

	BOOST_CHECK_EQUAL(body.getGravity(), 0.f);

	body.setGravity(13.37f);

	BOOST_CHECK_EQUAL(body.getGravity(), 13.37f);
}
BOOST_AUTO_TEST_CASE(BodyTest_SetPosition)
{
	Body body = Body(1.f, nullptr, false, false);

	BOOST_CHECK_EQUAL(body.getPosition().x, 0.f);
	BOOST_CHECK_EQUAL(body.getPosition().y, 0.f);
	BOOST_CHECK_EQUAL(body.getPosition().z, 0.f);
	BOOST_CHECK_EQUAL(body.getPosition().w, 1.f);

	body.setPosition(DirectX::XMFLOAT4(5.f, 10.f, 15.f, 1.f));
	body.setPosition(DirectX::XMFLOAT4(5.f, 10.f, 15.f, 1.f));

	BOOST_CHECK_EQUAL(body.getPosition().x, 5.f);
	BOOST_CHECK_EQUAL(body.getPosition().y, 10.f);
	BOOST_CHECK_EQUAL(body.getPosition().z, 15.f);
	BOOST_CHECK_EQUAL(body.getPosition().w, 1.f);
}


BOOST_AUTO_TEST_CASE(BodyTest_Update)
{
	Body body = Body(1.f, nullptr, false, false);

	DirectX::XMFLOAT4 m_Acceleration = body.getACC();
	BOOST_CHECK_EQUAL(m_Acceleration.x, 0.f);
	BOOST_CHECK_EQUAL(m_Acceleration.y, 0.f);
	BOOST_CHECK_EQUAL(m_Acceleration.z, 0.f);

	body.addForce(DirectX::XMFLOAT4(5.f, 0.f, 0.f, 0.f));

	float timestep = 1.f / 60.f;

	body.update(timestep);

	m_Acceleration = body.getACC();
	BOOST_CHECK_EQUAL(m_Acceleration.x, 5.f);
	BOOST_CHECK_EQUAL(m_Acceleration.y, 0.f);
	BOOST_CHECK_EQUAL(m_Acceleration.z, 0.f);

	DirectX::XMFLOAT4 m_LastAcceleration = body.getLastACC();
	BOOST_CHECK_EQUAL(m_LastAcceleration.x, 0.f);
	BOOST_CHECK_EQUAL(m_LastAcceleration.y, 0.f);
	BOOST_CHECK_EQUAL(m_LastAcceleration.z, 0.f);

	body.update(timestep);

	m_LastAcceleration = body.getLastACC();
	BOOST_CHECK_EQUAL(m_LastAcceleration.x, 2.5f);
	BOOST_CHECK_EQUAL(m_LastAcceleration.y, 0.f);
	BOOST_CHECK_EQUAL(m_LastAcceleration.z, 0.f);

	Body body2;
	body2 = std::move(body);
	m_LastAcceleration = body2.getLastACC();
	BOOST_CHECK_EQUAL(m_LastAcceleration.x, 2.5f);
	BOOST_CHECK_EQUAL(m_LastAcceleration.y, 0.f);
	BOOST_CHECK_EQUAL(m_LastAcceleration.z, 0.f);
}

BOOST_AUTO_TEST_CASE(BodyTest_Impulse)
{
	Body body = Body(1.f, nullptr, false, false);

	DirectX::XMFLOAT4 impulse = DirectX::XMFLOAT4(50.f, 0.f, 0.f, 0.f);
	body.addImpulse(impulse);
	BOOST_CHECK_GE(body.getACC().x, 0.f);
	BOOST_CHECK_EQUAL(body.getACC().x, 0.f);
	BOOST_CHECK_EQUAL(body.getACC().x, 0.f);

	body = Body(0.f, nullptr, false, false);

	body.addImpulse(impulse);
	BOOST_CHECK_EQUAL(body.getACC().x, 0.f);
	BOOST_CHECK_EQUAL(body.getACC().y, 0.f);
	BOOST_CHECK_EQUAL(body.getACC().z, 0.f);
}


BOOST_AUTO_TEST_CASE(BodyTest_Volumes)
{
	BoundingVolume::ptr fakeVolume(new DummyBoundingVolume());

	Body body = Body(1.f, std::move(fakeVolume), false, false);
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(0.1f);
	body.setRotation(rot);
	BOOST_CHECK_EQUAL(body.getVolumeListSize(), 1);
	BoundingVolume::ptr fakeVolume2(new DummyBoundingVolume());
	body.addVolume(std::move(fakeVolume2));
	BOOST_CHECK_EQUAL(body.getVolumeListSize(), 2);
	
	BOOST_CHECK(body.getCollisionResponse(0));

	body.setCollisionResponse(1, false);
	BOOST_CHECK(!body.getCollisionResponse(1));

	body.setCollisionResponse(true);
	BOOST_CHECK(body.getCollisionResponse(1));

	body.setVolumePosition(0, DirectX::XMVectorSet(1.f, 2.f, 3.f, 1.f));

	DummyBoundingVolume *volume = (DummyBoundingVolume*)body.getVolume();
	DirectX::XMFLOAT4 pos = volume->getPosition();
	BOOST_CHECK_EQUAL(pos.x, 1.f);
	BOOST_CHECK_EQUAL(pos.y, 2.f);
	BOOST_CHECK_EQUAL(pos.z, 3.f);

	DummyBoundingVolume *volume2 = (DummyBoundingVolume*)body.getVolume(1);
	DirectX::XMFLOAT4 pos2 = volume2->getPosition();
	BOOST_CHECK(pos2.x != 1.f);
	BOOST_CHECK(pos2.y != 2.f);
	BOOST_CHECK(pos2.z != 3.f);

	
}

BOOST_AUTO_TEST_CASE(BodyTest_Gets_Sets)
{
	Body body = Body(1.f, nullptr, false, false);
	
	body.setLanded(true);
	BOOST_CHECK(body.getLanded());

	body.setOnSomething(true);
	BOOST_CHECK(body.getOnSomething());

	BOOST_CHECK(!body.getIsEdge());
	BOOST_CHECK(!body.getIsImmovable());

	body.setVelocity(DirectX::XMFLOAT4(0.f, 0.f, 5.f, 0.f));
	DirectX::XMFLOAT4 vel = body.getVelocity();
	BOOST_CHECK_EQUAL(vel.x, 0.f);
	BOOST_CHECK_EQUAL(vel.y, 0.f);
	BOOST_CHECK_EQUAL(vel.z, 5.f);

	Body body2 = Body(1.f, nullptr, false, false);
	Body body3 = Body(1.f, nullptr, false, false);

	body.resetBodyHandleCounter();
	Body body4 = Body(1.f, nullptr, false, false);

	BOOST_CHECK_EQUAL(body4.getHandle(), 1);
}

BOOST_AUTO_TEST_SUITE_END()