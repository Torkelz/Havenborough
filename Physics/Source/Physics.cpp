#include "Physics.h"

using namespace DirectX;

_declspec(dllexport) IPhysics::ptr IPhysics::createPhysics()
{
	return IPhysics::ptr(new Physics());
}

Physics::Physics()
	: m_GlobalGravity(9.82f)
{

}

Physics::~Physics()
{

}

Body* Physics::findBody(BodyHandle p_Body)
{
	for (Body& b : m_Bodies)
	{
		if (b.getHandle() == p_Body)
		{
			return &b;
		}
	}

	return nullptr;
}

void Physics::initialize()
{
	m_Collision = Collision();
}

void Physics::update(float p_DeltaTime)
{
	m_HitDatas.clear();
	for(unsigned i = 0; i < m_Bodies.size(); i++)
	{
		Body& b = m_Bodies[i];

		if(b.getIsImmovable())
			continue;

		if(b.getInAir())
		{
			b.setGravity(m_GlobalGravity);
		}
		else
		{
			b.setGravity(0.f);
		}

		b.update(p_DeltaTime);

		bool onSomething = false;

		for (unsigned j = i + 1; j < m_Bodies.size(); j++)
		{
			Collision::HitData hit = m_Collision.boundingVolumeVsBoundingVolume(b.getVolume(), m_Bodies[j].getVolume());
			

			if(hit.intersect)
			{
				m_HitDatas.push_back(hit);
				XMVECTOR temp;
				XMFLOAT4 tempPos;

				temp = XMLoadFloat4(&b.getPosition()) + XMLoadFloat4(&hit.colNorm) * hit.colLength;
				XMStoreFloat4(&tempPos, temp);

				b.setPosition(tempPos);

				if (hit.colNorm.y > 0.88f)
				{
					onSomething = true;

					XMFLOAT4 velocity = b.getVelocity();
					velocity.y = 0.f;
					b.setVelocity(velocity);
				}
			}
		}

		b.setInAir(!onSomething);

		/*if(b.m_IsImmovable())
		{
			Collision col;
			for(SPD &s : m_spatialList)
				col.collision(b.getVolume, s.getVolume)
		}*/
	}
}

void Physics::applyForce(XMFLOAT4 p_force, BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return;

	body->addForce(p_force);
}

Physics::BodyHandle Physics::createBody(float p_Mass, BoundingVolume* p_BoundingVolumeType, bool p_IsImmovable)
{
	m_Bodies.emplace_back(p_Mass, std::unique_ptr<BoundingVolume>(p_BoundingVolumeType), p_IsImmovable);
	return m_Bodies.back().getHandle();
}

void Physics::setGlobalGravity(float p_Gravity)
{
	m_GlobalGravity = p_Gravity;
}

BoundingVolume* Physics::getVolume(BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return nullptr;

	return body->getVolume();
}

XMFLOAT4 Physics::getVelocity(BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	return body->getVelocity();
}

Collision::HitData Physics::getHitDataAt(unsigned int p_Index)
{
	return m_HitDatas.at(p_Index);
}

unsigned int Physics::getHitDataSize()
{
	return m_HitDatas.size();
}


//Debugging function
void Physics::moveBodyPosition(XMFLOAT4 p_Position, BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return;

	body->setPosition(p_Position);
	body->setVelocity(XMFLOAT4(0.f, 0.f, 0.f, 0.f));
}

//
//Collision::HitData Physics::sphereVsSphere( Sphere* p_sphere1, Sphere* p_sphere2 )
//{
//	return m_Collision.sphereVsSphere(p_sphere1, p_sphere2);
//}
//
//Collision::HitData Physics::AABBvsAABB( AABB* p_aabb1, AABB* p_aabb2 )
//{
//	return m_Collision.AABBvsAABB(p_aabb1, p_aabb2);
//}
//
//Collision::HitData Physics::AABBvsSphere( AABB* p_aabb, Sphere* p_sphere )
//{
//	return m_Collision.AABBvsSphere(p_aabb, p_sphere);
//}